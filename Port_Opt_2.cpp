void GetData ();
void Proposal ();
void Metropolis ();
double VarF(double **, double **);
double Probabilities (double somedelta);



// "ticker" is a global variable.
char **ticker;

// The covariance matrix is a global variable.
double **V;

// More Global Variables
double T; // Temperature

// Portfolio Global Variables
double **portx; //portfolio
double **porty; //proposed portfolio
double **RealMVP; //actual MVP calculated using matrices

#include "F.h"
#include <iostream>

int main () {

    int i, j, seed;
    int Accepted, AcceptTransition;
    portx = Array(50,1); //Portfolio array
    porty = Array(50,1); //Proposed portfolio array
    double **E = Array(50,1); //elementary matrix
    double **Var, **C, **VInv;

    // Read in and display the tickers and covariance matrix.
    GetData ();


    // Initial portfolio
    for (i = 1; i < 51; ++i){
        portx[i][1] = 0;
    }
    portx[1][1]=100; //initial state is 100% allocation in first stock

    for (int i = 1; i < 51; ++i) { // copies port x into y, we then make changes to y to create the proposal
        porty[i][1] = portx[i][1];
    }

    //Get the temperature
    std::cout << "What is the temperature to start with? (0 is best) ";
    std::cin >> T;

    // run the metropolis algorithm
    Metropolis();

    // Report the best-found portfolio and its variance here.
    std::cout << "\n";
    Show(portx);

    // Pause so the execution window does not close.
    Exit ();

}

////////////////////////////////////////////////////////////////////////////////
// Find the MVP via Metropolis. ////////////////////////////////////////////
void Metropolis () {

    int  AcceptTransition , n=0;
    double E, deltaE;
    // Get the temperature parameter.


    // Time the computations.
    // Time ();

    // Record initial energy.
    E = VarF(portx, V);

    // Continue as long as there is improvement to be had.
    while (E > 4.02856) {

        // Count the number of Markov chain steps.
        n ++;
        //std::cout << E << " This is E \n";

        // Periodically report the energy.
        if (n % 10000 == 0) {
          //  printf ("%3d  %3d\n", n, E);
        std::cout << E << " Current energy is \n";
        }

        // Determine the proposed transition.
        Proposal ();

        // Compute the change in energy.
        deltaE = VarF(porty, V) - VarF(portx, V);

        // Determine if proposed transition is accepted.
        AcceptTransition = 0;
        if (deltaE <= 0) {
            AcceptTransition = 1;
        }
            // If T > 0, accept a transition to higher energy with probability prob[deltaE].
        else if (T > 0) {
            if (MTUniform (0) <= Probabilities(deltaE)) {
                AcceptTransition = 1;
            }
        }
        // Change the state and update the energy if the proposed transition is accepted.
        if (AcceptTransition) {

            for (int i = 1; i < 51; ++i){
                portx[i][1] = porty[i][1];
            }

            E += deltaE;
        }

    }

    // Show n when E = 0.
    printf ("Solved after %d steps.  Computations took %.2f seconds", n, Time ());

}

////////////////////////////////////////////////////////////////////////////////
// This function calculates the variance of the current portfolio. /////////////
////////////////////////////////////////////////////////////////////////////////
double VarF (double **x, double **V) {
    double **xt, **xtV, **xtVx, var;

    //loop through the port and if theres any negative values make var = 1000 and return var
    for (int j = 1; j < 51; j++) {
        if(x[j][1] < -0.005){
            var = 1000;
            return(var);
        }
    }


    xt = Transpose (x);
    xtV = Multiply (xt, V);
    xtVx = Multiply (xtV, x);
    var = xtVx[1][1];
    Free(xt);
    Free(xtV);
    Free(xtVx);

    return var;

}

////////////////////////////////////////////////////////////////////////////////
// Function to propose a portfolio change by adding and subtracting $.01 //////
////////////////////////////////////////////////////////////////////////////////
void Proposal () {
    int i, j;
    for (i = 1; i < 51; ++i) { // copies the stuff in port x into y, we then make changes to y to create a proposal
        porty[i][1] = portx[i][1];
    }
    i = 50 * MTUniform(0);
    j = 50 * MTUniform(0);
    while (i == j) {
        i = 50 * MTUniform(0);
        j = 50 * MTUniform(0);
    }
    if (i < 50) { i = i + 1; }
    if (j < 50) { j = j + 1; }
    porty[i][1] -= .01; //subtracts one cent from a stock randomly chosen
    porty[j][1] += .01; //adds one cent to a stock randomly chosen
    //y is now the proposed portfolio
}

////////////////////////////////////////////////////////////////////////////////
// Compute the acceptance probabilities at temperature T. //////////////////////
////////////////////////////////////////////////////////////////////////////////
double Probabilities (double somedelta) {
    return(exp(-somedelta/T));
}

////////////////////////////////////////////////////////////////////////////////
// Allocate space for and read in covariance matrix and stock tickers.
////////////////////////////////////////////////////////////////////////////////
void GetData () {

    int i, j;
    double V0;
    char input[100];
    FILE *fp;

    // Allocate array space.
    V = Array (50, 50);

    // Allocate space to hold ticker names; "ticker" is a global variable.
    ticker = (char **) calloc (50+1, sizeof (char *));
    for (i = 0; i <= 50; i++) {
        ticker[i] = (char *) calloc (10, sizeof (char));
    }

    // Read in the data.
    fp = fopen ("V.txt", "r");
    for (i = 1; i <= 50; i++) {

        // Read in stock i's covariance data.

        // Name of the stock ticker.
        fgets (ticker[i], 9, fp);

        // The 50 covariances for stock "i".
        for (j = 1; j <= 50; j++) {

            // Read in V[i][j].
            fgets (input, 99, fp);
            sscanf (input, "%lf", &V0);

            // Put data into the V array.
            V[i][j] = V0;

        }

    }
    fclose (fp);

    return;

}



