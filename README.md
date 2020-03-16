## particles.c

A C implementation of [particles.js](https://github.com/cubified/particles.js) using ncurses.  Works very well as a terminal screensaver (a la `cmatrix`)

### Building and Running

Assuming `libncurses` is installed, simply run:

     make

Which should produce a binary named `particles`.  Additionally, this binary can be installed to `~/.local/bin` with:

     make install

### Overview (near-direct copy from the Javascript README)

All particles hold the following properties:

```
typedef struct particle {
  char type;

  double pos_x;
  double pos_y;

  double pos_x_last;
  double pos_y_last;

  double vel_x;
  double vel_y;

  double mass;
} particle;
```

On each frame, each particle's position is updated using its velocity vector, and its velocity is updated as follows:

- Initialize the particle's net force vector as 0 in both the x and y directions
- Iterate over every other particle, calculating the distance and angle of depression/elevation for each
- If the particles being evaluated are within some arbitrary distance, add to the net force vector using the following formula:
  - `(sign(particle2->pos_x or ->pos_y - particle1->pos_x or ->pos_y) * gravitational_constant * interaction_force)/dist(particle2, particle1)`
  - In English:
    - The particle that is past the other should experience a leftward force, while the particle that is behind the other should experience a rightward force (`sign(...)`)
    - The attractive/repulsive strength decreases as the distance between the two particles increases (`interaction_force/dist(...)`)
- Add the x and y components of the net force vector to the x and y components of the velocity respectively, dividing first by the particle's mass

In this particular implementation, attraction/repulsion force is:

- 1 (attractive) if two particles are of the same type
- -1 (repulsive) if the two particles' parity (evenness or oddness) are at odds
- 0 (nothing) if neither of two above conditions have been met
- Flipped if the F key is pressed

Although this is entirely arbitrary (and it remains a to-do to change this).

### To-Do

- Use CLI arguments to modify simulation variables
- Optimize for speed
- Investigate odd bug causing velocity to exceed `MAX_VELOCITY`
