Forward Park and Clarke Transformations

The FOC algorithm needs the I_d and I_q currents, calculated from a, b and c, which are the currents that go through each phase of the brushless motor. To do this, we need two transformations: the Park Transformation and the Clarke Transformation. 

Clarke Transformation

We have to project into alpha and beta axis (two new two-axis cartesian system) the 120° out of phase a, b and c vectors. With simple trigonometry we get:

![logo](../../../Media/clarke2eq.png)

Which is:

![logo](../../../Media/clarke2neweq.png)

We know because of Kirchhoff that:

![logo](../../../Media/kircheq.png)

So we substitute I_c with -I_a-I_b. Developing:

![logo](../../../Media/1clarkedev.png)

![logo](../../../Media/2clarkedev.png)

Then we multiply by 2/3 to ensure the transformation is amplitude-invariant:

![logo](../../../Media/23simp.png)

With those equations we have to read just two ADCs instead of three in the ESP32-S3 SuperMini, saving both pins and processing time.

Park Transformation

