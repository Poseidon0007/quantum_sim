#include <iostream>
#include <complex>
#include <cmath>
#include <utility> 
#include <random>

//lets create a qubit 
//a qubit has two complex amplitudes, alpha and beta, which represent the probability amplitudes for the states |0> and |1> respectively. 
// The sum of the squared magnitudes of these amplitudes must equal 1, which ensures that the qubit is in a valid quantum state.
class Qubit{
public:
    Qubit(){
        this->alpha = std::complex<double>(1.0, 0.0); // initialize to |0>
        this->beta = std::complex<double>(0.0, 0.0);
    }

    Qubit(std::complex<double> alpha, std::complex<double> beta){
        this->alpha = alpha;
        this->beta = beta;
    }

    double ProbabilityofZero(){
        return std::norm(alpha); // returns the squared magnitude of alpha
    } 

    double ProbabilityofOne(){
        return std::norm(beta); // returns the squared magnitude of beta
    }

    bool isNormalized(){
        double NormalizationCheck = std::norm(alpha) + std::norm(beta);
        // Check if the sum of squared magnitudes equals 1
        //tolerance for floating point comparison
        return (std::abs(NormalizationCheck - 1.0) < 1e-10); // checks if the sum of squared magnitudes equals 1
    }

    void printState(){
        std::cout << "Qubit state: " << alpha << "|0> + " << beta << "|1>" << std::endl;
        std::cout << "Probability of |0>: " << ProbabilityofZero() << std::endl;
        std::cout << "Probability of |1>: " << ProbabilityofOne() << std::endl;
        std::cout << "isNormalized: " << (isNormalized() ? "Yes" : "No") << std::endl;
    }

    // Apply the X gate (also known as the NOT gate) to the qubit, which swaps the amplitudes of |0> and |1>
    void applyx(){
        std::swap(alpha, beta); // swap the amplitudes for |0> and |1>
    }

    void applyy(){
        auto temp = alpha;
        auto i = std::complex<double>{0.0, 1.0};
        alpha=-i*beta;
        beta = i*temp;
    }

    void applyz(){
        beta=-beta;
    }


    //a hadamard gate is a single qubit operation that creates superposition. 
    // It transforms the basis states |0> and |1> into equal superpositions of both states. 
    void applyhadamard(){
        std::complex<double> new_alpha = (alpha + beta) / std::sqrt(2.0);
        std::complex<double> new_beta = (alpha - beta) / std::sqrt(2.0);
        alpha = new_alpha;
        beta = new_beta;
    }

    int measure(std::mt19937& engine){
        double prob_zero = ProbabilityofZero();
        std::uniform_real_distribution<double> distribution(0.0, 1.0);
        const double random = distribution(engine);

        if(prob_zero>random){
            alpha = std::complex<double>(1.0, 0.0);
            beta = std::complex<double>(0.0, 0.0);
            return 0;
        }
        else{
            alpha = std::complex<double>(0.0, 0.0);
            beta = std::complex<double>(1.0, 0.0);
            return 1;
        }
    }
private:
        std::complex<double> alpha; // amplitude for |0>
        std::complex<double> beta;  // amplitude for |1>
};

int main(){
    const double amplitude = 1.0 / std::sqrt(2.0);
    //Qubit q{
    //std::complex<double>{amplitude, 0.0},
    //std::complex<double>{amplitude, 0.0}
//};

    Qubit q;
    q.applyy();
    q.applyy();
    q.printState();
    
    // std::mt19937 engine{std::random_device{}()};
    // int zero_count=0;
    // int one_count=0;
    // for(int i=0;i<1000;i++){
    //     Qubit q{std::complex<double>{1.0, 0.0}, std::complex<double>{0.0, 0.0}};
    //     q.applyhadamard(); 
    //     int result=q.measure(engine);
    //     if(result){
    //         one_count++;
    //     }
    //     else{
    //         zero_count++;
    //     }
    // }
    // std::cout<<zero_count<<" "<<one_count<<std::endl;
}