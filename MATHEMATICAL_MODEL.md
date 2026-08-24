# Mathematical Model of the 2D Heat-Diffusion Simulator

This document derives the finite-difference scheme implemented by the C++
simulation. The goal is to approximate the temperature distribution on a
two-dimensional square plate.

## 1. Continuous problem

Let

$$
u=u(x,y,t)
$$

denote the temperature at position $(x,y)$ and time $t$. In a homogeneous
isotropic plate without internal heat sources, the temperature satisfies the
two-dimensional heat equation

$$
\frac{\partial u}{\partial t} = \alpha \nabla^2u =
\alpha\left(
\frac{\partial^2u}{\partial x^2}
+
\frac{\partial^2u}{\partial y^2}
\right),
$$

where $\alpha>0$ is the thermal diffusivity of the material.

## 2. Space and time grids

The plate is discretized on a uniform Cartesian grid:

$$
x_i=i\,\Delta x,
\qquad
y_j=j\,\Delta y,
\qquad
t_n=n\,\Delta t.
$$

The numerical approximation of the temperature is denoted by

$$
u_{i,j}^{n}\approx u(x_i,y_j,t_n).
$$

For a square uniform grid, we set

$$
\Delta x=\Delta y=h.
$$

## 3. Approximation of the time derivative

Using a forward difference in time,

$$
\frac{\partial u}{\partial t}(x_i,y_j,t_n)
\approx
\frac{u_{i,j}^{n+1}-u_{i,j}^{n}}{\Delta t}.
$$

Therefore,

$$
u_{i,j}^{n+1}
\approx
u_{i,j}^{n}
+
\Delta t\,
\frac{\partial u}{\partial t}(x_i,y_j,t_n).
$$

Substituting the heat equation gives

$$
u_{i,j}^{n+1}
\approx
u_{i,j}^{n}
+
\alpha\Delta t\,\nabla^2u(x_i,y_j,t_n).
$$

## 4. Approximation of the second derivative in the $x$ direction

Consider the approximate slopes to the left and right of $(x_i,y_j)$:

$$
m_L\approx
\frac{u_{i,j}^{n}-u_{i-1,j}^{n}}{\Delta x},
\qquad
m_R\approx
\frac{u_{i+1,j}^{n}-u_{i,j}^{n}}{\Delta x}.
$$

The second derivative measures the variation of the first derivative. Hence,

$$
\frac{\partial^2u}{\partial x^2}(x_i,y_j,t_n)
\approx
\frac{m_R-m_L}{\Delta x}.
$$

Substitution of the two slopes yields

$$
\frac{\partial^2u}{\partial x^2}(x_i,y_j,t_n) \approx \frac{1}{\Delta x} \left(\frac{u_{i+1,j}^{n}-u_{i,j}^{n}}{\Delta x} -
\frac{u_{i,j}^{n}-u_{i-1,j}^{n}}{\Delta x}
\right),
$$

and therefore

$$
\boxed{
\frac{\partial^2u}{\partial x^2}(x_i,y_j,t_n)
\approx
\frac{u_{i+1,j}^{n}-2u_{i,j}^{n}+u_{i-1,j}^{n}}
{(\Delta x)^2}
}.
$$

## 5. Approximation of the second derivative in the $y$ direction

The same argument in the vertical direction gives

$$
\boxed{
\frac{\partial^2u}{\partial y^2}(x_i,y_j,t_n)
\approx
\frac{u_{i,j+1}^{n}-2u_{i,j}^{n}+u_{i,j-1}^{n}}
{(\Delta y)^2}
}.
$$

## 6. Discrete Laplacian

Since $\Delta x=\Delta y=h$, adding the two centered approximations gives the
five-point discrete Laplacian:

$$
\nabla^2u(x_i,y_j,t_n)
\approx
\frac{1}{h^2}
\left(
u_{i+1,j}^{n}
+u_{i-1,j}^{n}
+u_{i,j+1}^{n}
+u_{i,j-1}^{n}
-4u_{i,j}^{n}
\right).
$$

The stencil uses the central cell and its four nearest neighbours:

```text
                  u(i,j+1)
                     |
        u(i-1,j) -- u(i,j) -- u(i+1,j)
                     |
                  u(i,j-1)
```

## 7. Explicit update formula

Substitution of the discrete Laplacian into the forward time approximation
gives

$$
u_{i,j}^{n+1} =
u_{i,j}^{n}
+
\frac{\alpha\Delta t}{h^2}
\left(
u_{i+1,j}^{n}
+u_{i-1,j}^{n}
+u_{i,j+1}^{n}
+u_{i,j-1}^{n}
-4u_{i,j}^{n}
\right).
$$

Define the dimensionless parameter

$$
r=\frac{\alpha\Delta t}{h^2}.
$$

The final explicit finite-difference scheme is

$$
\boxed{
u_{i,j}^{n+1}
= (1-4r)u_{i,j}^{n}
+r u_{i+1,j}^{n}
+r u_{i-1,j}^{n}
+r u_{i,j+1}^{n}
+r u_{i,j-1}^{n}
}.
$$

This is the formula evaluated by the `step()` function in the C++ program.

## 8. Stability condition

The coefficients in the update formula are

$$
1-4r,
\qquad r,
\qquad r,
\qquad r,
\qquad r,
$$

and their sum is one. To keep all weights non-negative, it is necessary that

$$
1-4r\geq0.
$$

Consequently, the explicit two-dimensional scheme is stable under the
condition

$$
\boxed{r\leq\frac{1}{4}}
$$

Equivalently, the time step must satisfy

$$
\boxed{
\Delta t\leq\frac{h^2}{4\alpha}
}.
$$

The C++ implementation uses

$$
r=0.2<\frac14.
$$

In this particular case,

$$
1-4r=0.2,
$$

so the update becomes

$$
u_{i,j}^{n+1}
= \frac15\left(
u_{i,j}^{n}
+u_{i+1,j}^{n}
+u_{i-1,j}^{n}
+u_{i,j+1}^{n}
+u_{i,j-1}^{n}
\right).
$$

Thus, at every time step, the new temperature is the average of the current
cell and its four nearest neighbours.

## 9. Initial and boundary conditions used in the simulation

The plate is initially at $20\,^{\circ}\mathrm{C}$, except for a square region
at its centre, which is initialized at $100\,^{\circ}\mathrm{C}$:

$$
u_{i,j}^{0}=
\begin{cases}
100, & (i,j)\text{ belongs to the central hot region},\\
20,  & \text{otherwise}.
\end{cases}
$$

The four edges are kept at the constant temperature $20\,^{\circ}\mathrm{C}$:

$$
u_{0,j}^{n}
=u_{N-1,j}^{n}
=u_{i,0}^{n}
=u_{i,N-1}^{n}
=20.
$$

These are fixed-temperature, or Dirichlet, boundary conditions. In the code,
they are enforced by updating only the interior indices

$$
1\leq i,j\leq N-2.
$$

## 10. Correspondence with the C++ implementation

The mathematical formula

$$
u_{i,j}^{n+1}$$
= (1-4r)u_{i,j}^{n}
+r u_{i+1,j}^{n}
+r u_{i-1,j}^{n}
+r u_{i,j+1}^{n}
+r u_{i,j-1}^{n}
$$

is implemented as

```cpp
next[center] =
    (1.0 - 4.0 * r) * current[center]
    + r * current[(i + 1) * N + j]
    + r * current[(i - 1) * N + j]
    + r * current[i * N + (j + 1)]
    + r * current[i * N + (j - 1)];
```

The one-dimensional position

$$
k=iN+j
$$

is used to store the two-dimensional cell $(i,j)$ in a contiguous C++ vector.
Two vectors are required: `current` stores $u^n$, while `next` stores
$u^{n+1}$. After all interior cells have been computed, their roles are
exchanged using

```cpp
std::swap(current, next);
```

This prevents values from the new time level from being used prematurely while
the same time step is still being calculated.

## 11. Accuracy

The forward difference in time has first-order accuracy, while the centered
differences in space have second-order accuracy. The local discretization is
therefore

$$
O(\Delta t)+O(h^2).
$$

The method is usually referred to as the **Forward-Time Centered-Space
(FTCS)** method with a **five-point stencil**.
