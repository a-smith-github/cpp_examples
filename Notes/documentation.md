# Thoughts on how to comment code

Comment style is perhaps the most subjective part of coding. However, it is the most essential for both individual and collaborative projects. Although it can make the code ugly, it’s always better to have too many comments than too few. The goal should be to always sustain maintainability such that there is very little wasted engineering effort for new starters or old hands having to context switch. Ideally someone with no prior knowledge of the project (but with 'sufficient' expertise in software development) can follow what is going on inside a program without needing to work it out for themselves or having to inspect many files.

This is difficult to achieve and needs to be performed in combination with good choices of variable, object, method, file names and intuitive project structure (single purpose files supported by headers, implementations in one or the other, one class per file etc.) and by keeping complexity low (as few classes as necessary with simple inheritance, relationships, design patterns). It's only through the combination of the two (perhaps with the assistance of documentation) that we can lower barriers to people wishing to contribute to a project. 

## Code Blocks and Methods

Blocks of code should do one obvious task within a method (e.g. applying a rotation matrix to an array) and should be supported by a comment line/block before the block of code begins. If splitting blocks follows a similar pattern to splitting paragraphs then the comment serves as an introduction and should be able to outline the task/data object/transformation the code is now focusing on (analogous to time/place/person). So to determine if an ellipsoid intercepts with another (and I follow the Alfano & Greer 2005 method), I would have code which 
- translates both ellipsoids such that one is centred in the laboratory frame (at 0,0,0)
- rotates both ellipsoids to the lab frame (so the axes of the centred ellipsoid aligns with the x,y,z axes)
- stretches both ellipsoids so the centred and aligned ellipsoid becomes a unit sphere (x',y',z' become 1,1,1)
- uses the transformed axes and coordinates of the non-centred ellipsoid to form a 4x4 matrix and solve for the eigenvalues to determine if there is an intercept

#### "Write a c++ program which translates, rotates and stretches two ellipsoids such that one ellipsoid becomes a unit sphere and determine if the two ellipsoids intercept by solving for the eigenvalues"
#### ChatGPT:

```c++
#include <iostream>
#include <cmath>
#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

int main()
{
  // Define the matrix A representing the first ellipsoid
  Matrix3d A;
  A << 1, 0, 0,
       0, 2, 0,
       0, 0, 3;

  // Define the matrix B representing the second ellipsoid
  Matrix3d B;
  B << 2, 1, 1,
       1, 3, 1,
       1, 1, 4;

  // Translate the first ellipsoid to the origin
  Vector3d t;
  t << -1, -1, -1;
  Matrix3d T;
  T << 1, 0, 0,
       0, 1, 0,
       0, 0, 1;
  T.col(2) = -t;
  A = T * A * T.transpose();

  // Rotate the first ellipsoid to align with the coordinate axes
  Matrix3d R;
  SelfAdjointEigenSolver<Matrix3d> eigensolver(A);
  if (eigensolver.info() == Success) {
    R = eigensolver.eigenvectors();
  } else {
    cerr << "Error: eigenvalue decomposition failed!" << endl;
    return -1;
  }
  A = R * A * R.transpose();

  // Stretch the first ellipsoid to become a unit sphere
  Vector3d s;
  s << 1.0 / sqrt(A(0,0)), 1.0 / sqrt(A(1,1)), 1.0 / sqrt(A(2,2));
  Matrix3d S;
  S << s(0), 0, 0,
       0, s(1), 0,
       0, 0, s(2);
  A = S * A * S.transpose();

  // Apply the same transformations to the second ellipsoid
  B = T * B * T.transpose();
  B = R * B * R.transpose();
  B = S * B * S.transpose();

  // Check if the two ellipsoids intercept by solving for the eigenvalues
  Matrix3d C = A - B;
  SelfAdjointEigenSolver<Matrix3d> eigensolver(C);
  if (eigensolver.info() == Success) {
    Vector3d ev = eigensolver.eigenvalues();
    if (ev.minCoeff() >= 0) {
      cout << "The two ellipsoids do not intercept." << endl;
    } else {
      cout << "The two ellipsoids intercept." << endl;
    }
  } else {
    cerr << "Error: eigenvalue decomposition failed!" << endl;
    return -1;
  }

  return 0;
}
```

Of course, it might have been better to put these blocks of codes in their own methods
#### "Can you do the same but put each step in a method?"
#### ChatGPT:

```c++
#include <iostream>
#include <cmath>
#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

Matrix3d translate(const Matrix3d& A, const Vector3d& t)
{
  Matrix3d T;
  T << 1, 0, 0,
       0, 1, 0,
       0, 0, 1;
  T.col(2) = -t;
  return T * A * T.transpose();
}

Matrix3d rotate(const Matrix3d& A)
{
  Matrix3d R;
  SelfAdjointEigenSolver<Matrix3d> eigensolver(A);
  if (eigensolver.info() == Success) {
    R = eigensolver.eigenvectors();
  } else {
    cerr << "Error: eigenvalue decomposition failed!" << endl;
    exit(-1);
  }
  return R * A * R.transpose();
}

Matrix3d stretch(const Matrix3d& A)
{
  Vector3d s;
  s << 1.0 / sqrt(A(0,0)), 1.0 / sqrt(A(1,1)), 1.0 / sqrt(A(2,2));
  Matrix3d S;
  S << s(0), 0, 0,
       0, s(1), 0,
       0, 0, s(2);
  return S * A * S.transpose();
}

bool intersect(const Matrix3d& A, const Matrix3d& B)
{
  Matrix3d C = A - B;
  SelfAdjointEigenSolver<Matrix3d> eigensolver(C);
  if (eigensolver.info() == Success) {
    Vector3d ev = eigensolver.eigenvalues();
    return (ev.minCoeff() < 0);
  } else {
    cerr << "Error: eigenvalue decomposition failed!" << endl;
    exit(-1);
  }
}
```

Which makes main very human-readable and even begins to resemble the steps of the method as laid out in the paper. 

```c++

int main()
{
  // Define the matrix A representing the first ellipsoid
  Matrix3d A;
  A << 1, 0, 0,
       0, 2, 0,
       0, 0, 3;

  // Define the matrix B representing the second ellipsoid
  Matrix3d B;
  B << 2, 1, 1,
       1, 3, 1,
       1, 1, 4;

  // Translate the first ellipsoid to the origin
  Vector3d t;
  t << -1, -1, -1;
  A = translate(A, t);

  // Rotate the first ellipsoid to align with the coordinate axes
  A = rotate(A);

  // Stretch the first ellipsoid to become a unit sphere
  A = stretch(A);

  // Apply the same transformations to the second ellipsoid
  B = translate(B, t);
  B = rotate(B);
  B = stretch(B);

  // Check if the two ellipsoids intercept
  bool intercept = intercept(A, B);
  
  return 0;
}
```
I personally think the eigenvalue step is not very clear and, in a few months, I won't really understand what is going on without having to refer to the paper. I don't really have a grasp of the intercept condition just from the looking at the code because it is obscured by the eigensolve step which requires specific domain knowledge. We should improve the comments in this scenario to look something like this:

## Doc strings

#### "Could you add more verbose comments in the intercept method about the values of the eigenvalues when ellipsoids do and do not intercept? Could you also reflect this in the docstring? Only show the intercept method and docstring"
#### ChatGPT:

```c++
/**
 * Determines if the two given ellipsoid matrices intercept.
 *
 * @param A The first ellipsoid matrix.
 * @param B The second ellipsoid matrix.
 *
 * @return True if the two ellipsoids intercept, false otherwise.
 *
 * The ellipsoids intercept if the matrix C = A - B has at least one
 * negative eigenvalue. This is because the ellipsoid equation is given by
 * x^TAx = 1 for some matrix A, and if C has at least one negative
 * eigenvalue, then there exists a vector x such that x^TAx = 1 and
 * x^TCx < 0, meaning that x is a point that lies inside both ellipsoids
 * A and B.
 */
bool intersect(const Matrix3d& A, const Matrix3d& B)
{
  Matrix3d C = A - B;
  SelfAdjointEigenSolver<Matrix3d> eigensolver(C);
  if (eigensolver.info() == Success) {
    Vector3d ev = eigensolver.eigenvalues();
    return (ev.minCoeff() < 0);
  } else {
    cerr << "Error: eigenvalue decomposition failed!" << endl;
    exit(-1);
  }
}
```

In addition we need to consider doc strings as a separate subclass of comment which provide in-code explanation of input/output variables and purpose of each method, class, etc.. So not only are we creating automatic documentation but we are also making it easier for the uninitiated to follow what the code is doing. Combined with a clear variable name describing the limited scope of the method, it should be unnecessary to inspect the blocks of code which form the method to determine what the method is doing to it’s inputs and what any output represents. If the method should throw an error, this should also be included in the doc string (e.g. @throws Error::Ellipsoid::InvalidCoordinate) and an explaination included in the description.

#### "Could you please add docstrings for each of the methods?"
#### ChatGPT:

```c++
/**
 * Translates the given ellipsoid matrix by the given translation vector.
 *
 * @param A The ellipsoid matrix to translate.
 * @param t The translation vector.
 *
 * @return The translated ellipsoid matrix.
 */
Matrix3d translate(const Matrix3d& A, const Vector3d& t)
{
...
}

/**
 * Rotates the given ellipsoid matrix to align with the coordinate axes.
 *
 * @param A The ellipsoid matrix to rotate.
 *
 * @return The rotated ellipsoid matrix.
 */
Matrix3d rotate(const Matrix3d& A)
{
...
}

/**
 * Stretches the given ellipsoid matrix to become a unit sphere.
 *
 * @param A The ellipsoid matrix to stretch.
 *
 * @return The stretched ellipsoid matrix.
 */
Matrix3d stretch(const Matrix3d& A)
{
...
}

/**
 * Determines if the two given ellipsoid matrices intercept.
 *
 * @param A The first ellipsoid matrix.
 * @param B The second ellipsoid matrix.
 *
 * @return True if the two ellipsoids intercept, false otherwise.
 */
bool intersect(const Matrix3d& A, const Matrix3d& B)
{
...
}
```
Compare the doc string for the intercept method above to the doc string at the top of the section. I don't believe there are many cases where a doc string that describes the inputs and outputs alone is sufficient --- even a method as simple as 'add' could benefit and would improve documentation. 


## Tests

We should also include comments which define the test specifications --- they could be similar to the doc strings for methods where the in/out variable explanation is replaced by the file dependencies (e.g. config files, model files), the target class/method/function/file which the test is designed to ensure works, the issue that the test proved was fixed, any conditions about the system for test success/failure (e.g. <test_name> fixes/adds feature support for issue207 and ensures volume of ellipsoid is unchanged after rotation of axes). This size of these lists will likely be different for unit tests and integration tests and some of these categories may not be applicable for all tests. The goal is to allow an uninitiated user to locate the point of failure when a test fails during CI or when validating a local install/compile and to aid anyone when reporting and fixing a bug. These are more important for integration tests where the test target is obscured by interaction with other code. 

#### "Could you write a test using Catch2 for ellipsoid pairs of varying size that tests the validity of your answer?" 
#### "Could you only show the TEST_CASE portion of the answer but provide a detailed test specification comment above the test?"

#### ChatGPT:
```c++
// Test the intersection of ellipsoids with varying size
// Inputs:
//   i: size of the first ellipsoid
//   j: size of the second ellipsoid
// Output:
//   result: true if the two ellipsoids intersect, false otherwise
TEST_CASE("Intersection of ellipsoids")
{
  // Define the translation vector
  Vector3d t;
  t << -1, -1, -1;

  // Test ellipsoid pairs with varying size
  for (int i = 1; i <= 10; ++i) {
    for (int j = 1; j <= 10; ++j) {
      // Define the matrix A representing the first ellipsoid
      Matrix3d A;
      A << i, 0, 0,
           0, i, 0,
           0, 0, i;

      // Define the matrix B representing the second ellipsoid
      Matrix3d B;
      B << j, 0, 0,
           0, j, 0,
           0, 0, j;

      // Translate the first ellipsoid to the origin
      A = translate(A, t);

      // Rotate the first ellipsoid to align with the coordinate axes
      A = rotate(A);

      // Stretch the first ellipsoid to become a unit sphere
      A = stretch(A);

      // Apply the same transformations to the second ellipsoid
      B = translate(B, t);
      B = rotate(B);
      B = stretch(B);

      // Check if the two ellipsoids intercept
      bool expected = (i < j);
      bool result = intersect(A, B);
      REQUIRE(result == expected);
    }
  }
}
```
I think the above would be a bare minimum example for a unit test. For an integration test you may also be testing the classes which hold the ellipsoids, their data access methods, as well as other aspects of your system such as periodic boundary conditions or test a specific ellipsoid (e.g. a minimum volume enclosing ellipsoid around a molecule) or some other system-dependent property, as well as any methods which you directly target (this list should be concise, small changes help here). You may wish to expand on the test specification. You may also wish to modify the language to say passes/fails rather than true/false and speak of parameters rather than inputs. You may also wish to include cases where intercepts should not occur and test accordingly. Although, in some cases, it is possible to determine when a test was added is the repository was properly managed, it is good practice to include the year or version that the test was made for in case this is information is lost --- this helps when removing technical debt or obsolete features in the future.

In the event of failure of a Catch2 test it would be useful to use a dynamic section so that `i` and `j` become test parameters and are included in the output.

```c++
// <test_name>: Tests the fix for Issue #207 so ellipsoids that intercepts for ellipsoids 
// that are unit spheres or smaller are detected across period boundary conditions.
// Parameters:
//   i: scaling factor of the first ellipsoid
//   j: scaling factor of the second ellipsoid
//
// Result: 
//   PASSES if the two ellipsoids intersect, FAILS otherwise
//
// Test targets:
//   Class Ellipsoid
//
// Test dependencies:
//   C60.mol
//   C1H4.mol
//
// Release:
//   v1.2

TEST_CASE("Intersection of ellipsoids")
{
  // Test ellipsoid pairs with varying size
  for (int i = 1; i <= 10; ++i) {
    for (int j = 1; j <= 10; ++j) {
    
      DYNAMIC_SECTION("For scaling factors " << i << " and " << j << " test ellipsoid intercepts for C60 and C1H4")
      {
...     

```

(Note: this might not be the correct way of formatting this information for an automatic documentation generation tool to ignore or process, this is just an example of what information is useful)

When a bug is identified it can either be fixed straight away or the test can get filtered (turned off so as to not break CI) and a follow-up task/issue created (note: in a small organisation where code is merged infrequently and CI tests are run on a per change basis, it would be more likely you would fix the bug rather than track an issue). Additionally, if you leave a TODO/FIXME in the code where the change is too large or outside the scope of the current work, you should also create a follow-up task/issue. In both cases the issue number should be included in the comment so that a quick search can be performed to help identify the requirements for that feature/issue and to make sure the issue has been fully addressed before making a submission/pull request. In some CI systems, a submission will be blocked or require a new follow-up task to be linked to the failure and other automatic checks will fail if the linked issue is present anywhere in the code base.


For a method 

```c++
void applyPeriodicBoundaryConditions(double& x, double& y, double& z, double boxSize)
```

you might write a unit test to validate functionality. Such a test could consist of multiple cases (for each dimension, test if a point initially placed outside (negative), inside, and outside (postive) is repositioned to inside the box). I have slightly modified the generated answer here for test case 3.

```c++
#include "catch.hpp"

// Include the header file for the applyPeriodicBoundaryConditions function
#include "applyPeriodicBoundaryConditions.h"

TEST_CASE("Periodic boundary conditions are applied correctly", "[periodic-boundary-conditions]") {
  double x, y, z;
  double boxSize;

  // Test case 1: x coordinate is outside the box in the positive direction
  x = 2.0;
  y = 1.0;
  z = 0.5;
  boxSize = 1.0;
  applyPeriodicBoundaryConditions(x, y, z, boxSize);
  REQUIRE(x == Approx(0.0));  // x should be wrapped around to the other side of the box
  REQUIRE(y == Approx(1.0));  // y should not change
  REQUIRE(z == Approx(0.5));  // z should not change

  // Test case 2: y coordinate is outside the box in the negative direction
  x = 0.5;
  y = -0.5;
  z = 1.0;
  boxSize = 1.0;
  applyPeriodicBoundaryConditions(x, y, z, boxSize);
  REQUIRE(x == Approx(0.5));  // x should not change
  REQUIRE(y == Approx(0.5));  // y should be wrapped around to the other side of the box
  REQUIRE(z == Approx(1.0));  // z should not change

  // Test case 3: test applyPeriodicBoundaryConditions for a z coordinate which is outside 
  // the box in the positive direction
  x = 0.25;
  y = 0.75;
  z = 1.25;
  boxSize = 1.0;
  applyPeriodicBoundaryConditions(x, y, z, boxSize);
  // x and y should remain unchanged
  REQUIRE(x == Approx(0.25));
  REQUIRE(y == Approx(0.75));
  // z should be wrapped around to the other side of the box
  REQUIRE(z == Approx(0.25));
}
```
Of course these tests could be split into their own TEST_CASEs but comments to aid specifying the success (and failure) conditions create a full specification for the method. In this case the include (and accompanying comment) makes clear which file is targeted. In the event that there are multiple includes required, separating out the targets or providing a written list in a separate comment aids any requirements analysis you might perform before making a change. In the absence of other documentation (such as a feature specification) this information (the interface and the test conditions) should be enough to describe the intent, requirements and any bugs/pain points that had to be resolved such that a new implementation could be written without re-introducing a regression. 

Although this is a contrived and inexhaustive example, in this case we know we have an ellipsoid intercept method which tests if there is overlap with another ellipsoid (an intercept) by comparing the ellipsoid matrices (axes) after relocating them in space and, following a bug, after applying periodic boundary conditions which behave as in the above test. If we looked at any auto-generated documentation we would know more about the implementation methodology and the meaning of the full range of return values from the intercept method. In principle we could write that comment in a header file (or even an abridged version of it), write the implementation in a source file and it would be unnecessary to see the source file to know how the method functioned.

You can never fully replace documentation but you can get close enough that, in a pinch, you can make a change without looking. This is also exactly how you enforce that other features that interact with your feature respect the intended behaviour of your feature. This is a defensive mechanism so support is not unintentionally eroded. If a test fails when they make a change, then it wasn't (necessarily) your fault and instead perhaps the new feature misunderstood the requirements and architectural change that you made. When that test fails, you should give them enough information so they know what rule they broke and what behaviour they need to restore so that your feature remains supported.
