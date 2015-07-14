#ifndef PHP_HPP
#define PHP_HPP

#include <controller.hpp>
#include <ph7.h>

class PHPManager : public QObject, public ControllerManager {
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "com.obsidian.ControllerManager" FILE "php.json")
	Q_INTERFACES(ControllerManager)

	public:
		explicit PHPManager(QObject* parent = nullptr);
		~PHPManager();
		QVariant execute(const Controller& , QObjectList&);
		bool has(const Controller&);

	protected:
		ph7_value* exposeToVM(QObject* obj, ph7_vm* pVm);
		static int nativeCall(ph7_context* ctx, int argc, ph7_value** argv);

	private:
		ph7* m_engine;
		QHash<QString, ph7_vm*> m_controllers;
		QObjectList m_objects;
};

#endif // PHP_HPP

