# NBL - New Bastiaan Language
This is a prototype interpreter for the NBL (New Bastiaan Language) programming language written in C11 C. It only uses the standard C library so it is very protable.

It is a mix of **JavaScript**, **PHP** and **Lua**. But not all the language features are implemented in the interpreter yet.

## Things todo:
- Type check variables
- Function default argument
- string array thing
- for in
    - array
    - object
- Free stuff: no mem leaks
- Tenary operator
- Expand stdlib
- Classes?
- Syntax hightlighting vscode?

## Types:
- `null`
- `boolean`
- `int`
- `float`
- `string`
- `array`
- `object`
- `function`

## Comments
```
# Single line comment

// Single line comment

/*
Multi line comment
*/
```

## Variables
```
let x = 0;
x = 1;
const y = 2;
y = 3; // Error: can't mutate a const variable
```

## Arrays
```
const names = [ 'Bastiaan', 'Jan', 'Dirk' ];
println(array_length(names)); // -> 3
println(names[1]); // -> 'Jan'
```

## Objects
```
const person = {
    name = 'Bastiaan',
    age = 17
};

println('Name = ' + person.name + ', Age = ' + (string)person['age'])
```

## Conditions
```
const number = 4.56;

if (number > 3) {
    println('Number is greater then 3!');
}
else if (number > 4) {

}
elseif (number > 4) {

}
elif (number > 4) {

}
else {

}
```

## Loops
```
// While loops
let x = 10;
while (x > 0) {
    x = x - 1;
    continue;
}

// Do while loops
let x = 0;
do {
    x = x + 1;
    break;
} while(x > 10);

// For loops
for (let i = 0; i < 100; i = i + 1) {
    println('-', i);
}

// For in loop
const names = [ 'Bastiaan', 'Dirk', 'Willem' ];
for (const name in names) {
    println(name);
}
```

## Functions
```
fn double(number) {
    return number * 2;
}

const add = fn (a: int, b: int): int {
    return a + b;
};

println(add(double(4), 4)); // -> 12
```
