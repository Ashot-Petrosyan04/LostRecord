# The LostRecord Programming Language Documentation

## 1. Program Structure

### 1.1 Sentences
Every complete statement, or "sentence," in LostRecord must end with a **period `.`**.

### 1.2 Comments
Single-line comments begin with `//`. The compiler ignores all text from `//` to the end of the line.

### 1.3 File Extension
Source code files for the LostRecord language should use the **`.lr`** extension to honor the idea of each program as a “lost record.”

**Example File: `legend.lr`**
```LostRecord
// This is a comment.
a value hero_level, type int, is revealed as 1. // This is a declaration.
```

## 2. Data Types & Literals
LostRecord supports three primary data types:

- **`int`**: A 64-bit whole number.
  - *Literals*: `42`, `-100`, `0`
- **`string`**: A sequence of characters.
  - *Literals*: `"Hello, world!"`, `"The dragon roars."`
- **`bool`**: A boolean value representing truth.
  - *Literals*: `true`, `false`

## 3. Variables & Values
Variables hold the data that your story manipulates. They are introduced using the `a value` declaration.

### 3.1 Mutable Variables
A variable whose value can be changed is declared using `begins at`.

> **Syntax:**
> ```LostRecord
> a value <name>, type <type>, begins at <expression>.
> ```

> **Example:**
> ```LostRecord
> a value health, type int, begins at 100.
> ```

### 3.2 Immutable Values (Constants)
A constant value that cannot be changed is declared using `is revealed as`.

> **Syntax:**
> ```LostRecord
> a value <name>, type <type>, is revealed as <expression>.
> ```

> **Example:**
> ```LostRecord
> a value max_score, type int, is revealed as 9999.
> ```

### 3.3 Changing a Value (Assignment)
To change the value of a mutable variable, use the `continues as` statement.

> **Syntax:**
> ```LostRecord
> the value <name> continues as <expression>.
> ```

> **Example:**
> ```LostRecord
> the value health continues as health minus 10.
> ```

## 4. Expressions
Expressions are phrases that compute a value. They are used in declarations, assignments, and other statements.

### 4.1 Arithmetic Operators
- `<left> plus <right>`: Addition
- `<left> minus <right>`: Subtraction
- `<left> multiplied by <right>`: Multiplication
- `<left> divided by <right>`: Division

### 4.2 Comparison Operators
Comparisons evaluate to a `bool` value (`true` or `false`) and are primarily used in control flow.
- `<left> is greater than <right>`
- `<left> is less than <right>`
- `<left> is equal to <right>`

### 4.3 Logical Operators
Logical operators work on `bool` values.
- `<left> and <right>`: Logical AND
- `<left> or <right>`: Logical OR
- `not <expression>`: Logical NOT

### 4.4 Function Calls
A call to a function that returns a value can be used as an expression.

> **Syntax:**
> ```LostRecord
> the story of '<function_name>' using (<argument1>, <argument2>)
> ```

> **Example:**
> ```LostRecord
> a value new_score, type int, begins at score plus the story of 'calculate_bonus' using (level).
> ```

## 5. Control Flow
### 5.1 Conditional Statements (if)
An `if` statement executes a block of code only if a condition is true.

> **Syntax:**
> ```LostRecord
> if <condition> is met, tell the following story:
> beginning of the story
>     <body statements>
> end of the story.
> ```

> **Example:**
> ```LostRecord
> if health is less than 1 is met, tell the following story:
> beginning of the story
>     the story tells: "You have been defeated.".
> end of the story.
> ```

### 5.2 Loops (while)
A `while` loop repeatedly executes a block of code as long as a condition remains true.

> **Syntax:**
> ```LostRecord
> while <condition> holds, tell the following story:
> beginning of the story
>     <body statements>
> end of the story.
> ```

> **Example:**
> ```LostRecord
> while enemies is greater than 0 holds, tell the following story:
> beginning of the story
>     the value enemies continues as enemies minus 1.
> end of the story.
> ```

### 5.3 Early Loop Exit (break)
You can exit a `while` loop early using a special statement.

> **Syntax:**
> ```LostRecord
> the story ends at this moment.
> ```

> **Example:**
> ```LostRecord
> while true holds, tell the following story:
> beginning of the story
>     // ...
>     if special_item_found is equal to true is met, tell the following story:
>     beginning of the story
>         the story ends at this moment.
>     end of the story.
> end of the story.
> ```

## 6. Input & Output
The language provides built-in statements for writing to the console.

### 6.1 Printing a Value
Use `the story tells:` to print the value of any expression.

> **Syntax:**
> ```LostRecord
> the story tells: <expression>.
> ```

> **Example:**
> ```LostRecord
> the story tells: "Your final score is: ".
> the story tells: score.
> ```

### 6.2 Printing a Newline
Use `the story ends a line.` to print a newline character.

> **Syntax:**
> ```LostRecord
> the story ends a line.
> ```

## 7. Procedures & Functions
Procedures are reusable blocks of code. Functions are procedures that return a value.

### 7.1 Declaration
Procedures are declared using `for procedure named ... accepting`.
Functions add the `and yielding` clause to specify a return type.

> **Syntax (Procedure):**
> ```LostRecord
> for procedure named '<name>' accepting (<param1> as <type1>), tell the following story:
> beginning of the story
>     <body statements>
> end of the story.
> ```

> **Syntax (Function):**
> ```LostRecord
> for procedure named '<name>' accepting (<param1> as <type1>) and yielding <return_type>, tell the following story:
> beginning of the story
>     <body statements>
> end of the story.
> ```

### 7.2 Returning a Value
Inside a function, use `the result shall be` to return a value. This must be the last statement executed in a path.

> **Syntax:**
> ```LostRecord
> the result shall be <expression>.
> ```

> **Example:**
> ```LostRecord
> for procedure named 'get_double' accepting (n as int) and yielding int, tell the following story:
> beginning of the story
>     the result shall be n multiplied by 2.
> end of the story.
> ```

### 7.3 Calling a Procedure
To execute a procedure that does not return a value, use the `perform` statement.

> **Syntax:**
> ```LostRecord
> perform the story of '<procedure_name>' using (<argument1>, <argument2>).
> ```

> **Example:**
> ```LostRecord
> perform the story of 'show_inventory' using (player_name).
> ```

## 8. Scoping
LostRecord uses lexical scoping. A new scope is created for each procedure or function.

- Variables and parameters declared inside a procedure are local to that procedure.
- They cannot be accessed from outside the procedure.
- This allows different procedures to use the same variable names without conflict.