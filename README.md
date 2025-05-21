# Dependency Injection Framework - First variant
Further referred to as **diff**.

## Make a difference
_Diff_ is a C++ framework targeted for embedded systems. It provides a structured way to design and implement software - making it **modular**, **maintainable** and **reusable**. 

It supports software **development process** from multiple perspectives:
- Directs **architects** to create modular design,
- Narrows the playground for **development** and **QA** to focus on single problem at a time,
- Lets **non-technical** team members to understand the design and even compose and configure the final application binary.

All that with **CI/CD** automation in mind. 

TODO Image.

The framework uses [Dependency Injection](https://en.wikipedia.org/wiki/Dependency_injection) design pattern on top of user-provided set of [software components](https://en.wikipedia.org/wiki/Software_component). Components in diff serve as **building blocks** for the final design. Each component provides a solution to a distinct problem, leveraging decomposition. Each is designed and implemented independently. Each can be tested individually and any set of components can be tested together if needed. At the end, the binary is composed of - at this point - off-the-shelf components, following a topology described by a **human-readable Json** file. 

## Diff components, modules and instances
Diff operates on two component types:
- **Component** is a single C++ class which encapsulates a specific functionality. It holds the implementation. Its internal architecture may be complex, utilizing various other classes and libraries. The class itsself is only visible to the framework. If certain functionality needs to be exposed or utilized, it must be done by implementing or consuming an _interface component_.
- **Interface Component** is a single abstract class which determines how certain functionality shall be used. It does not consist the information on how it is implemented. The functionality represented by _interface component_ can be either implemented and/or consumed by a regular _component_. _Interface components_ serve as joint points between regular _components_. For this reason they shall consist of as little code as possible (to make it bug-proof) - ideally only an abstract interface.

**Module** is a physical representation of a component. Regular _components_ are shipped in form of static or dynamic libraries. Since _interface components_ are just interface declarations, they come in the form of header-only libraries. Diff refers to these libraries as **modules**. They are a subject for versioning and dependency management, where the exact tools used (e.g. Conan 2) are up to the team and the project.

Each _module_ can be built and tested on its own. There is no dependency chain, as _components_ only depend on _interface components_ and its internally used tools. 

Framework creates **Component Instances** to compose a hierarchy required by the application. Each _component_ can be instantiated multiple times.

## Requirements
Diff requires **C++14** or higher. It utilizes both the language features and also several STL containers provided by the standard. 

As of today the choice of C++14 is a sweet point between taking advantage of modern C++ features and ensuring that almost all targeted toolchains provide the required standard (which is not necessarily true for C++17). 
