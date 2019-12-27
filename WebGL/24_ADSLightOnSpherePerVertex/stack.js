// Stack class 
class Stack { 

	// Array is used to implement stack 
	constructor() 
	{ 
		this.items = []; 
	} 
    // push function 
    push(element) 
    { 
        // push element into the items 
        this.items.push(element); 
    } 
    // pop function 
    pop() 
    { 
        // return top most element in the stack 
        // and removes it from the stack 
        // Underflow if stack is empty 
        if (this.items.length == 0) 
            return "Underflow"; 
        return this.items.pop(); 
    } 
    // peek function 
    peek() 
    { 
        // return the top most element from the stack 
        // but does'nt delete it. 
        return this.items[this.items.length - 1]; 
    } 

        // isEmpty function 
    isEmpty() 
    { 
        // return true if stack is empty 
        return this.items.length == 0; 
    } 

} 


// // This Stack is written using the pseudoclassical pattern

// // Creates a stack
// var Stack = function() {
//     this.count = 0;
//     this.storage = {};
// }

// // Adds a value onto the end of the stack
// Stack.prototype.push = function(value) {
//     this.storage[this.count] = value;
//     this.count++;
// }

// // Removes and returns the value at the end of the stack
// Stack.prototype.pop = function() {
//     // Check to see if the stack is empty
//     if (this.count === 0) {
//         return undefined;
//     }

//     this.count--;
//     var result = this.storage[this.count];
//     delete this.storage[this.count];
//     return result;
// }

// // Returns the length of the stack
// Stack.prototype.size = function() {
//     return this.count;
// }