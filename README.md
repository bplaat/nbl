# New Bastiaan Language
This is a prototype interpreter for the NBL (New Bastiaan Language) programming language written in C99 C. It only uses the standard C library so it is very protable.

The language has the syntax of **JavaScript**, the strong but dynamic type system of **Lua** en the standard functions of **PHP**. But not all the language features are implemented in the interpreter yet.

## Types:
- `null`
- `number`
- `string`
- `boolean`
- `array`
- `object`
- `function`

## Operations
```
variable = value
variable (number) += number
variable (number) -= number
variable (number) *= number
variable (number) **= number
variable (number) /= number
variable (number) %= number

variable (string) += string
variable (string) += null
variable (string) += number
variable (string) += boolean

+ number    ->    number
- number    ->    number
number + number    ->    number
number - number    ->    number
number * number    ->    number
number ** number    ->    number
number / number    ->    number
number % number    ->    number

string + string    ->    string
null + string    ->    string
string + null    ->    string
number + string    ->    string
string + number    ->    string
boolean + string    ->    string
string + boolean    ->    string

null == null    ->    boolean
number == number    ->    boolean
boolean == boolean    ->    boolean
string == string    ->    boolean

null != null    ->    boolean
number != number    ->    boolean
boolean != boolean    ->    boolean
string != string    ->    boolean

number > number    ->    boolean
number >= number    ->    boolean
number < number    ->    boolean
number <= number    ->    boolean

! boolean    ->    boolean
boolean && boolean    ->    boolean
boolean || boolean    ->    boolean
```

## Comments
```js
# Single line comment

// Single line comment

/*
Multi line comment
*/
```

## Arrays
```js
names = [ 'Bastiaan', 'Jan', 'Dirk' ]

array_length(names)    ->    3

names[0]    ->    'Bastiaan'
```

## Objects
```js
person = {
    name = 'Bastiaan',
    age = 17
};

println('Name = ' + person.name + ', Age = ' + person['age'])
```

## Conditions
```js
number = 4.56

if (number > 3) {
    println('Number is greater then 3!')
}
 (number > 4) {

}
else {

}
```

## Loops
```js
// While loops
x = 10;
while (x > 0) {
    x = x - 1;
    continue;
}

// Do while loops
x = 0;
do {
    x = x + 1;
    break;
} while(x > 10);

// For loops
for (i = 0; i < 100; i += 1) {
    println('- ' + i);
}

// For in loop
names = [ 'Bastiaan', 'Dirk', 'Willem' ]
for (name in names) {
    println(name)
}
```

## Functions
```js
function double(number) {
    return number * 2;
}

add = function (a, b) {
    return a + b;
}

array = {
    length = function (array) {
        length = 0;
        while (array[length] != null) {
            length += 1;
        }
        return length;
    }
}

println(add(3, 4));
```
