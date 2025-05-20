# Dependency Injection Framework - First variant
Further referred to as **_diff_**.

## Make a difference
_Diff_ is a C++ framework targeted for embedded systems. It provides a structured way to design and implement software - making it **modular**, **maintainable** and **reusable**. 

_Diff_ supports the whole **development process** from multiple perspectives:
- Directs **architects** to create modular design,
- Narrows the playground for **development** and **QA** to focus on single problem at a time,
- Lets **non-technical** team members to understand the design and even compose and configure the final application binary.

All that with **CI/CD** automation in mind. 

TODO Image.

_Diff_ uses [Dependency Injection](https://en.wikipedia.org/wiki/Dependency_injection) design pattern on top of user-provided set of [software components](https://en.wikipedia.org/wiki/Software_component). _Diff components_ serve as building blocks for the design. Each component provides a solution to a distinct problem, leveraging decomposition. Each is designed and implemented independently. Each can be tested individually and any set of components can be tested together if needed. At the end, the resulting binary is composed of - at this point, already - off-the-shelf components. 

## Diff components, instances and modules
_Diff_ uses two types of components - **Implementation Components** (or simply **Components**) and **Interface Components**. They come in **Modules**, which are their physical representations. Lastly, the framework creates **Component Instances** to compose a hierarchy required by the application binary.

- **Component** (or _Implementation Component_) in _diff_ is a single C++ class that encapsulates a specific functionality. Its internal architecture may be complex, utilizing various other classes and libraries.
- **Interface Component** is

TODO

. It operates on components. Each component is implemented in form of a C++ class, but not every class
is a component. Components in diff 

In general a  consists of an internal implementation - which solves some kind of a problem - and an interface - which defines how to use the underlying implementation.  For this reason diff defines two component types:
Diff operates on components, implemented in form of C++ classes.  


Diff operates on two types of components:
- **Component** (or _Implementation component_ for better distinction) - 
    - Implements (zero or more) interfaces defined by _interface components_,
    - Consumes (zero or more) interfaces defined by _interface components_,
    - Consists of an implementation for a solution of a particular problem,
    - From the outside perspective - implemented in form of a C++ class,
    - From the inside perspective - may use other classes, libraries and have more complex architecture.

- **Interface component** - Defines a single interface, which _implementation components_ may either implement and/or consume.  

A [component](https://en.wikipedia.org/wiki/Software_component) in general is a  
form a hierarchy and interact with each other. There are two kinds of components:
- _Implementation component_, or simply a _Component_ - A set of functionalities implemented in form of a c++ class. It may consist of multiple internal classes, files, libraries and complex architecture on the inside.    
A component in the context of _diff_ 

## Requirements
Diff requires **C++14** or higher. It utilizes both the language features and also several STL containers provided by the standard. 

As of today the choice of C++14 is a sweet point between taking advantage of modern C++ features and ensuring that almost all targeted toolchains provide the required standard (which is not necessarily true for C++17). 
