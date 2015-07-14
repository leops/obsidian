#include <php.hpp>
#include <ph7.c>
#include <httprequest.hpp>
#include <httpresponse.hpp>
#include <global.hpp>
#include <obsidian.hpp>

#include <QMetaProperty>
#include <QMetaMethod>

static int Output_Consumer(const void* pOutput, unsigned int nOutputLen, void* pUserData) {
	Q_UNUSED(pUserData)
	printf("%.*s\n", nOutputLen, (const char*) pOutput);
	return PH7_OK;
}

ph7_value* PHPManager::exposeToVM(QObject* obj, ph7_vm* pVm) {
	auto pClass = PH7_VmExtractClass(pVm, "QObject", sizeof("QObject") - 1, 0, 0);
	if(pClass == 0)
		return nullptr;

	auto pStd = PH7_NewClassInstance(pVm, pClass);
	if(pStd == 0)
		return nullptr;

	auto pCons = PH7_ClassExtractMethod(pClass, "__construct", sizeof("__construct") - 1);
	if(pCons) {
		ph7_value* apArg[1];
		apArg[0] = ph7_new_scalar(pVm);
		ph7_value_int(apArg[0], m_objects.length());

		m_objects.append(obj);

		PH7_VmCallClassMethod(pVm, pStd, pCons, nullptr, 1, apArg);
	}

	ph7_value* pObj = ph7_new_scalar(pVm);
	pObj->x.pOther = pStd;
	MemObjSetType(pObj, MEMOBJ_OBJ);
	return pObj;
}

ph7_value* toValue(ph7_context* ctx, const QVariant& val) {
	auto ret = ph7_context_new_scalar(ctx);
	switch(val.type()) {
		case QMetaType::Int:
			ph7_value_int(ret, val.toInt());
			break;
		case QMetaType::LongLong:
			ph7_value_int64(ret, val.toLongLong());
			break;
		case QMetaType::Bool:
			ph7_value_bool(ret, val.toBool());
			break;
		case QMetaType::Double:
			ph7_value_double(ret, val.toDouble());
			break;
		case QMetaType::QString: {
			auto str = val.toString().toLatin1();
			ph7_value_string(ret, str.constData(), str.size());
		} break;
		case QMetaType::QVariantHash: {
			ph7_context_release_value(ctx, ret);

			auto hash = val.toHash();
			auto arr = ph7_new_array(ctx->pVm);
			for(auto i = hash.constBegin(); i != hash.constEnd(); ++i)
				ph7_array_add_strkey_elem(arr, i.key().toLatin1().constData(), toValue(ctx, i.value()));

			return arr;
		}; break;
		case QMetaType::QVariantList: {
			ph7_context_release_value(ctx, ret);

			auto list = val.toList();
			auto arr = ph7_new_array(ctx->pVm);
			for(auto i = 0; i < list.size(); i++)
				ph7_array_add_intkey_elem(arr, i, toValue(ctx, list.value(i)));

			return arr;
		}; break;
		default:
			ph7_value_null(ret);
			break;
	}
	return ret;
}

QVariant toVariant(ph7_value* val) {
	if(ph7_value_is_int(val))
		return QVariant::fromValue(ph7_value_to_int(val));
	else if(ph7_value_is_bool(val))
		return QVariant::fromValue(ph7_value_to_bool(val));
	else if(ph7_value_is_float(val))
		return QVariant::fromValue(ph7_value_to_double(val));
	else if(ph7_value_is_string(val))
		return QVariant(ph7_value_to_string(val, nullptr));
	else if(ph7_value_is_array(val)) {
		QVariantHash hash;
		ph7_array_walk(val, [](ph7_value* k, ph7_value* v, void* data) -> int {
			auto hash = static_cast<QVariantHash*>(data);
			QString key(ph7_value_to_string(k, nullptr));

			hash->insert(key, toVariant(v));

			return PH7_OK;
		}, &hash);
		return hash;
	} else
		return QVariant();
}

template<typename T>
T invokeMethod(QObject* obj, const QMetaMethod& method, QList<QGenericArgument> args) {
	T ret;
	bool res = method.invoke(obj, Qt::DirectConnection,
				  QReturnArgument<T>(QMetaType::typeName(method.returnType()), ret),
				  args.value(0, QGenericArgument()),
				  args.value(1, QGenericArgument()),
				  args.value(2, QGenericArgument()),
				  args.value(3, QGenericArgument()),
				  args.value(4, QGenericArgument()),
				  args.value(5, QGenericArgument()),
				  args.value(6, QGenericArgument()),
				  args.value(7, QGenericArgument()),
				  args.value(8, QGenericArgument()),
				  args.value(9, QGenericArgument()));
	if(!res)
		qWarning() << "Method not invoked";
	return ret;
}

int PHPManager::nativeCall(ph7_context* ctx, int argc, ph7_value** argv) {
	if(argc < 3)
		return PH7_ABORT;

	auto self = static_cast<PHPManager*>(ph7_context_user_data(ctx));
	auto obj = self->m_objects.at(ph7_value_to_int(argv[0]));
	auto name = ph7_value_to_string(argv[1], nullptr);

	typedef std::tuple<QList<QGenericArgument>, QList<QVariant>, QStringList> ArgumentList;
	ArgumentList data;

	ph7_array_walk(argv[2], [](ph7_value* key, ph7_value* val, void* d) -> int {
		Q_UNUSED(key)
		auto data = static_cast<ArgumentList*>(d);

		std::get<1>(*data).append(toVariant(val));

		auto& last = std::get<1>(*data).last();
		auto typeName = QMetaType::typeName(last.type());

		std::get<0>(*data).append(QGenericArgument(typeName, last.data()));
		std::get<2>(*data).append(typeName);

		return PH7_OK;
	}, &data);

	auto mo = obj->metaObject();
	auto sig = QString("%1(%2)").arg(name).arg(std::get<2>(data).join(','));
	auto index = mo->indexOfMethod(sig.toLatin1().constData());
	if(index != -1) {
		auto method = mo->method(index);
		QMetaType::Type rType = static_cast<QMetaType::Type>(method.returnType());
		switch(rType) {
			case QMetaType::Int:
				ph7_result_int(ctx, invokeMethod<int>(obj, method, std::get<0>(data)));
				break;
			case QMetaType::Bool:
				ph7_result_bool(ctx, invokeMethod<bool>(obj, method, std::get<0>(data)));
				break;
			case QMetaType::Double:
				ph7_result_double(ctx, invokeMethod<double>(obj, method, std::get<0>(data)));
				break;
			case QMetaType::QString: {
				auto str = invokeMethod<QString>(obj, method, std::get<0>(data)).toLatin1();
				ph7_result_string(ctx, str.constData(), str.size());
			} break;
			case QMetaType::QVariantHash: {
				auto hash = invokeMethod<QVariantHash>(obj, method, std::get<0>(data));
				ph7_result_value(ctx, toValue(ctx, hash));
			}; break;
			case QMetaType::QVariantList: {
				auto list = invokeMethod<QVariantList>(obj, method, std::get<0>(data));
				ph7_result_value(ctx, toValue(ctx, list));
			}; break;
			default:
				qWarning().nospace().noquote() << "Unsupported type " << rType << " for " << mo->className() << "::" << sig;
			case QMetaType::Void:
				method.invoke(obj, Qt::DirectConnection,
							  std::get<0>(data).value(0, QGenericArgument()),
							  std::get<0>(data).value(1, QGenericArgument()),
							  std::get<0>(data).value(2, QGenericArgument()),
							  std::get<0>(data).value(3, QGenericArgument()),
							  std::get<0>(data).value(4, QGenericArgument()),
							  std::get<0>(data).value(5, QGenericArgument()),
							  std::get<0>(data).value(6, QGenericArgument()),
							  std::get<0>(data).value(7, QGenericArgument()),
							  std::get<0>(data).value(8, QGenericArgument()),
							  std::get<0>(data).value(9, QGenericArgument()));
				break;
		}
	} else {
		qDebug() << "Method not found" << sig << "in" << mo->className();
	}

	return PH7_OK;
}

PHPManager::PHPManager(QObject* parent) : QObject(parent) {
	int rc = ph7_init(&m_engine);
	if(rc != PH7_OK) {
		qFatal("Error while allocating a new PH7 engine instance");
	}

	auto libCode = QByteArray().append("<?php\n")
			.append("class QObject {\n")
				.append("private $_id;\n")
				.append("public function __construct($id) {$this->_id = $id;}\n")
				.append("public function __get($name) {return __native_get($this->_id, $name);}\n")
				.append("public function __set($name, $value) {return __native_set($this->_id, $name, $value);}\n")
				.append("public function __call($name, $args) {return __native_call($this->_id, $name, $args);}\n")
			.append("}\n")
		.append("?>");

	auto app = dynamic_cast<Obsidian*>(Obsidian::instance());

	auto ctrlDir = getDir("controllers");
	Q_FOREACH (QString fileName, ctrlDir.entryList({"*.php"}, QDir::Files)) {
		auto name = fileName.split(".").first().toLower();
		qDebug().noquote() << "Loading controller" << name;
		QFile script(ctrlDir.absoluteFilePath(fileName));
		script.open(QIODevice::ReadOnly);

		ph7_vm* ctrl;
		auto code = libCode + script.readAll();
		switch(ph7_compile_v2(m_engine, code.constData(), code.length(), &ctrl, 0)) {
			case PH7_OK:
				m_controllers.insert(name, ctrl);
				break;
			case PH7_COMPILE_ERR:
				const char *zErrLog;
				int nLen;
				ph7_config(m_engine, PH7_CONFIG_ERR_LOG, &zErrLog, &nLen);
				if(nLen > 0) {
					puts(zErrLog);
				}
			default:
				qWarning() << "Compile error";
				break;
		}

		if(ph7_vm_config(ctrl, PH7_VM_CONFIG_OUTPUT, Output_Consumer, 0) != PH7_OK) {
			qWarning() << "Error while installing the VM output consumer callback";
		}

		ph7_create_function(ctrl, "__native_get", [](ph7_context* ctx, int argc, ph7_value** argv) -> int {
			if(argc < 2)
				return PH7_ABORT;

			auto self = static_cast<PHPManager*>(ph7_context_user_data(ctx));
			auto obj = self->m_objects.at(ph7_value_to_int(argv[0]));
			auto val = obj->property(ph7_value_to_string(argv[1], nullptr));
			ph7_result_value(ctx, toValue(ctx, val));

			return PH7_OK;
		}, this);

		ph7_create_function(ctrl, "__native_set", [](ph7_context* ctx, int argc, ph7_value** argv) -> int {
			if(argc < 3)
				return PH7_ABORT;

			auto self = static_cast<PHPManager*>(ph7_context_user_data(ctx));
			auto obj = self->m_objects.at(ph7_value_to_int(argv[0]));
			obj->setProperty(ph7_value_to_string(argv[1], nullptr), toVariant(argv[1]));
			ph7_result_null(ctx);

			return PH7_OK;
		}, this);

		ph7_create_function(ctrl, "__native_call", &PHPManager::nativeCall, this);

		if(app) {
			auto modelList = app->getModels();
			Q_FOREACH(auto manager, modelList) {
				auto list = manager->models();
				for(int i = 0; i < list.length(); i++) {
					auto model = dynamic_cast<QObject*>(manager->models(list[i]));
					if(model) {
						ph7_create_constant(ctrl, list[i].toLatin1().constData(), [](ph7_value* val, void* d){
							auto data = static_cast<ph7_value*>(d);
							*val = *data;
						}, exposeToVM(model, ctrl));
					}
				}
			}
		}

		script.close();
	}
}

PHPManager::~PHPManager() {
	Q_FOREACH (ph7_vm* vm, m_controllers)
		ph7_vm_release(vm);
	ph7_release(m_engine);
}

QVariant PHPManager::execute(const Controller& ctrl, QObjectList& args) {
	QString className = std::get<0>(ctrl),
			methodName = std::get<1>(ctrl);
	QVariant res = 500;

	if(!m_controllers.contains(className))
		return false;

	ph7_value sResult;
	auto pVm = m_controllers.value(className);
	PH7_MemObjInit(pVm, &sResult);

	ph7_value mName;
	auto data = methodName.toLatin1();
	SyString mNVal {
		data.constData(),
		static_cast<unsigned int>(data.size())
	};
	PH7_MemObjInitFromString(pVm, &mName, &mNVal);

	QVector<ph7_value*> mArgs;
	Q_FOREACH(auto arg, args)
		mArgs.append(exposeToVM(arg, pVm));

	sResult.nIdx = SXU32_HIGH;
	sxi32 rc = PH7_VmCallUserFunction(pVm, &mName, args.length(), mArgs.data(), &sResult);
	if(rc == SXRET_OK) {
		res = ph7_value_to_int(&sResult);
	}

	PH7_MemObjRelease(&sResult);

	return res;
}

bool PHPManager::has(const Controller& ctrl) {
	QString className = std::get<0>(ctrl),
			methodName = std::get<1>(ctrl);

	if(!m_controllers.contains(className))
		return false;

	auto pVm = m_controllers.value(className);
	auto mName = methodName.toLatin1();
	if( SyHashGet(&pVm->hFunction, (const void *) mName.constData(), (sxu32) mName.size()) != 0 ||
		SyHashGet(&pVm->hHostFunction, (const void *) mName.constData(), (sxu32) mName.size()) != 0 ){
		return true;
	}

	return false;
}
