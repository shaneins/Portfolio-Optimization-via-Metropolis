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

#include "Functions.h"
#include <iostream>

int main () {

    int i, j, seed;
    int Accepted, AcceptTransition;
    portx = Array(50,2); //Portfolio array (Now 2 columns, 1 if included, 0 if not)
    porty = Array(50,2); //Proposed portfolio array (Now 2 columns, 1 if included, 0 if not)
    double **E = Array(50,1); //elementary matrix
    double **Var, **C, **VInv;

    // Read in and display the tickers and covariance matrix.
    GetData ();

    // Initial portfolio (start with every stock included each ticker gets  $2)

    portx[25][1] = 100;
    portx[25][2] = 1;

    for (int i = 1; i < 51; ++i) { // copies the stuff in port x into y, we then make changes to y to create a proposal
        porty[i][1] = portx[i][1];
        porty[i][2] = portx[i][2];
    }

    //Get the temperature
    T=0;
    // run the metropolis algorithm
    Metropolis();

    // Report the best-found portfolio and its variance here.
    Show(portx);

    std::cout << "The final energy of the least energy portfolio is " << VarF(portx, V);
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
    Time ();

    // Record initial energy.
    E = VarF(portx, V);
    std::cout << E;

    while (E > 4.45385) {

        // Count the number of Markov chain steps.
        n ++;
        //std::cout << E << " This is E \n";

        // Periodically report the energy.
        if (n % 1000 == 0) {
          //  printf ("%3d  %3d\n", n, E);
        std::cout << E << " Current energy is \n";
        }

        // Determine the proposed transition.
        Proposal ();

        // Compute the change in energy.
        deltaE = VarF(porty, V) - VarF(portx, V);
        //std::cout << deltaE << " This is deltaE \n";

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
                portx[i][2] = porty[i][2];
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

    //loop through port if theres any negative set var = to 1000 and return
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
void Proposal () { // keep in mind the constraints as well as the concepts of neighbors
    int i;
    double j=0;
    double count;

    for (i = 1; i < 51; ++i) { // copies the stuff in port x into y, we then make changes to y to create a proposal
        porty[i][2] = portx[i][2];
    }

    i = 50 * MTUniform(0);
    if (i < 50) { i = i + 1;}
    if (porty[i][2] == 0) {porty[i][2] = 1;}
        else {porty[i][2] = 0;}
    for (i = 1; i < 51; ++i){ //count the amount of stocks in port
        j+=porty[i][2];
    }
    
    j = 100.0/j; //Determine equal allocation for all stocks
    for (i = 1; i < 51; ++i){
        if (porty[i][2]==1) {porty[i][1] = j;}
            else {porty[i][1]=0;}
    }

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



