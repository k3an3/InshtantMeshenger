InshtantMeshenger Coding Standards
----------------------------------

Heavily based on the Linux Coding syle guidelines
https://www.kernel.org/doc/Documentation/CodingStyle

## Brace style

Adhere to the Linux Kernel Coding standards for brace
style. This means that all non-function block statements
(if, switch, for, while) should look like this:

  if (test) {
  	stuff;
  }

Functions, however, are special:

  int *
  function(int x)
  {
  	body;
  }

This is because functions *are* special.

Use a "cuddled else"

  if (condition) {
	a;
  } else if (condition) {
  	b;
  } else {
  	c;
  }

Do not use braces for single line if statements, unless
it is part of an if, else if, else. In that case, absolutely DO
use braces on all components. An else should always contain braces.

## Function declarations

Since we will end up using some rather complicated types,
type information goes on the line preceding the function declaration
or definition, as show above.

## Indentation

Use tabs. Rationale: Brain cells

## Column Width

Maintain 80 character columns, assuming a tab width of 4 columns. This is a
compromise between the 8 column tradition, as has been established for over
half a certury, and the practical limitations of C++ as a very highly indented
language.

However, because we are using tab characters rather than heretical spaces, if
you happen to have more than 80 columns at your disposal, you can increase
the width of tabs to make viewing easier. Remember to adhere to the 80 column
restriction, and this restriction assumes 4 column tabs.

## Parethesis and spacing

Put a space before the parenthesis for if, while, etc.

  if (condition)

But not for functions, even declarations/definitions

  int
  foo(int x);
 
  if (foo(10))

If you put spaces on the inside of parenthesis, you are a disappointment to
humanity, and your offspring will perish.

## Naming

Use ALL_CAPS for constants, UpperCamelCase for classes, lowerCamelCase for 
class members and local variables.

Methods should also be in lowerCamelCase. Do not do m_membername for members;
they should be private so it doesn't matter if
they "look like methods". If you really feel like one
should be public, it's probably time to rethink what you are trying to do
or overload operator=.

## Scope/Encapsulation

Members should be private. This makes threading easier, and keeps code
clean. SetX() and GetX() are ugly warts. Try to avoid designing yourself
into a situation where they are necessary.

## RAII

Adhere to RAII. If your method isn't exception-safe, you should go work
for Google instead.
