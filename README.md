Obsidian
====================================

# Technology
Obsidian is a web-framework created in C++, allowing for low-level optimization
and amazing performances. It's based on the Qt framework, used for TCP
communications abstraction, memory management, but also for its plugin system.

# Modules
This is the main feature of Obsidian. The naked framework only handles the HTTP
request and performs routing. Everything else is left for plugins. Obsidian is
build arround the Model-View-Controller pattern, and this is exactly what a
module can do.

Modules are dynamic libraries that implement one of the 3 extension interfaces
(ModelManager, ControllerManager or ViewManager), and are then loaded into the
main program.

To give an example of the possibilities, some plugins are included in the
project:
- AssetsManager, a "virtual" controller manager to quickly serve static files
- ScriptManager, a controller manager built arround Qt's V8 integration for
  writing controllers in EcmaScript
- MustacheManager, a view manager to render views written with the mustache
  templating language
- PHPManager (unstable), a controller manager using PH7 to execute controllers
  created using PHP
- SQLManager, a model manager for SQL databases
