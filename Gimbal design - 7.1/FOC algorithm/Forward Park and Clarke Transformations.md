Forward Park and Clarke Transformations

The FOC algorithm needs the I_d and I_q currents, calculated from a, b and c, which are the currents that go through each pin of the brushless motor. To do this, we need two transformations: the Park Transformation and the Clarke Transformation. 

Clarke Transformation

We have to project into alpha and beta axis (two new cartesian axis) the 120° out of phase a, b and c vectors. With simple trigonometry we get:

![logo](./Media/clarke2eq.png)

We know because of Kirchhoff that:

![logo](./Media/kirchhoffeq.png)

So we substitute I_c with -I_a-I_b. Then we simplify multiplying by 2/3:

![logo](./Media/23simplification.png)

That way we just have to read two ADCs of the S3 instead of three.

Park Transformation

