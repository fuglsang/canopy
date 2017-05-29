# canopy
*canopy* is a light-weight C++ framework with strong emphasis on *performance* and *joy of use*. It grows from a desire to have a solid and good looking toolbox with nice and trustworthy tools in it; to support own experiments and rapid application development.

## !!! WORK IN PROGRESS !!!
Currently in the process of refactoring and adding in components from older version of same framework. Feel free to poke around, but don't expect that anything works. Repository is largely a skeleton at the moment.

#### CODE CONVENTIONS
Like C, but with namespaces and template meta-programming.

* root namespace is `ca`
* no C++ STL containers
* no `new` or `delete`
* no member functions
* no constructors
* no exceptions
* `thread_t` is a type
* type modifiers are written right-to-left so they are trivial to read (e.g. `thread_t const *` reads pointer to const thread)
* `thread_join(thread_t *)` is a function
* `{` is written on next line
* `sys_thread.h` is a filename
* first token in filename denotes sub-root namespace of contents (e.g. `thread_t` lives in namespace `ca::sys`)

#### BUILDING ON WINDOWS
1. git clone https://github.com/fuglsang/canopy.git
2. open canopy.sln with VS2017
3. hit "Build/Build Solution"
4. static library is built to lib/
5. headers can be found in include/ca/

#### AUTHOR
Lasse Jon Fuglsang Pedersen <<volmer@gmail.com>>

#### LICENSE
BSD 2-Clause (see [LICENSE](LICENSE))