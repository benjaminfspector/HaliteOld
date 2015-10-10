# Halite

Halite is a game played by AIs. Halite is built in C++ using OpenGL 2.0. It is primarily developed by [Benjamin Spector](https://github.com/Sydriax "Benjamin Spector") with help from [Michael Truell](https://github.com/truell20 "Michael Truell").

Halite includes a lightweight library for writing AIs in C++. We are working on porting Halite to Java and Python.

## The Rules

Halite is played on a grid of variable size. Each square in the grid is eighter empty, a player's sentient piece, or a player's factory piece. 

Factory squares age each turn. When a factory piece's age reaches the grids sentient age (a constant determined by the size of the grid), the factory piece resets its age and creates a sentient piece on top of it. 

Each turn, every player has the option to move each of their sentient pieces either up, down, left, or right to an adjacent position on the grid. The grid is wrap-around, so there are no boarders; pieces may always move up, down, left, or right. When a player's sentient piece moves onto an empty square or an opponent's factory piece, that square becomes a factory piece for the player who owns the sentient. When a sentient piece moves onto a friendly factory square, that factory square's age is reset. When two sentients move adjacent to one another, both sentients are killed, and so, are removed from the game.

The goal of Halite is to be the last player in the game. An AI is knocked out of the game if they have no pieces left.

## The AIs

Through the development of Halite, we and our friends have made a number of AIs and pitted them against one another. Here is a quick summary of some of the most important:

- Random AI: Moves each sentient randomnly. The first AI built
- Basic AI: Moves each sentient to an empty square or an opponent's factory square if adjacent. If the sentient borders neither of these, it moves the sentient randomnly.
- Diffusion AI: Each sentient is repulsed by all other sentients proportional to each pieces distance. Each sentient moves away from the vector sum of this repulsion.
- Hybrid AI: If a sentient borders an empty piece or an opponent's factory piece, the Diffusion AI's logic is applied. If however, the sentient is in the middle of your territory, it moves to the closest empty piece or opposing factory piece.
- Neural Network AI: This AI uses the code from [Michael Truell and Josh Gruenstein's Fido project](https://github.com/truell20/Fido-Public "Michael Truell and Josh Gruenstein's Fido project") to move sentients according to the descisions of an artificial neural network. The neural network was given the pieces surrounding each sentient and which direction to move the sentient or to keep the sentient still.
