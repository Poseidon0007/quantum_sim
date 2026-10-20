#include <iostream>
#include <complex>
#include <cmath>
#include <utility> 
#include <random>
#include <vector>
#include <string>

enum class SingleQubitGate { X, Y, Z, H };
enum class TwoQubitGate { HFirst, HSecond, CnotFirstControl, CnotSecondControl };

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

class TwoQubit{

public:
    TwoQubit(){
        this->alpha = std::complex<double>(1.0, 0.0); 
        this->beta = std::complex<double>(0.0, 0.0);
        this->gamma = std::complex<double>(0.0, 0.0); 
        this->delta = std::complex<double>(0.0,0.0);
    }

    void applyHadamardToFirst(){
        const auto new_alpha = (alpha + gamma) / std::sqrt(2.0);
        const auto new_beta = (beta + delta) / std::sqrt(2.0);
        const auto new_gamma = (alpha - gamma) / std::sqrt(2.0);
        const auto new_delta = (beta - delta) / std::sqrt(2.0);

        alpha = new_alpha;
        beta = new_beta;
        gamma = new_gamma;
        delta = new_delta;
    }

    void applyHadamardToSecond(){
        const auto new_alpha = (alpha + beta) / std::sqrt(2.0);
        const auto new_beta = (alpha - beta) / std::sqrt(2.0);
        const auto new_gamma = (gamma + delta) / std::sqrt(2.0);
        const auto new_delta = (gamma - delta) / std::sqrt(2.0);

        alpha = new_alpha;
        beta = new_beta;
        gamma = new_gamma;
        delta = new_delta;
    }

    //CNOT swaps gamma and delta
    void applyCNOTWithFirstAsControl(){
        std::swap(gamma,delta);
    }

    void applyCNOTWithSecondAsControl(){
        std::swap(beta,delta);
    }

    int measure(std::mt19937& engine){
        const double random = std::uniform_real_distribution<double>(0.0, 1.0)(engine);
        const double probability00 = std::norm(alpha);
        const double probability01 = std::norm(beta);
        const double probability10 = std::norm(gamma);

        if(random < probability00){
            alpha = {1.0, 0.0};
            beta = gamma = delta = {0.0, 0.0};
            return 0;
        }
        if(random < probability00 + probability01){
            beta = {1.0, 0.0};
            alpha = gamma = delta = {0.0, 0.0};
            return 1;
        }
        if(random < probability00 + probability01 + probability10){
            gamma = {1.0, 0.0};
            alpha = beta = delta = {0.0, 0.0};
            return 2;
        }

        delta = {1.0, 0.0};
        alpha = beta = gamma = {0.0, 0.0};
        return 3;
    }

    bool isNormalized(){
        double NormalizationCheck = std::norm(alpha) + std::norm(beta) + std::norm(gamma) + std::norm(delta);
        return (std::abs(NormalizationCheck - 1.0) < 1e-10); // checks if the sum of squared
    }

    void printState(){
        std::cout << "TwoQubit state: " << alpha << "|00> + " << beta << "|01> + " << gamma << "|10> + " << delta << "|11>"<< std::endl;
        std::cout << "Probability of |00>: " << std::norm(alpha) << std::endl;
        std::cout << "Probability of |01>: " << std::norm(beta) << std::endl;
        std::cout << "Probability of |10>: " <<std::norm(gamma) << std::endl;
        std::cout << "Probability of |11>: " <<std::norm(delta) << std::endl;
        std::cout << "isNormalized: " << (isNormalized() ? "Yes" : "No") << std::endl;
    }


private:
    std::complex<double> alpha;
    std::complex<double> beta;
    std::complex<double> gamma;
    std::complex<double>delta;
};

class execution{
private:
Qubit q;
TwoQubit tq;
std::vector<SingleQubitGate> SGate;
std::vector<TwoQubitGate> TGate;
public:
void prepare_single_qubit_gate_list(){
    std::cout << "Enter single-qubit gates in order (X, Y, Z, H). Enter N to finish.\n";
    bool add_gates=true;
    while(add_gates){
        char gate;
        if (!(std::cin >> gate)) {
            std::cout << "Input ended before the gate list was complete.\n";
            return;
        }
        switch(gate){
            case 'X':
                SGate.push_back(SingleQubitGate::X);
                break;
            case 'Y':
                SGate.push_back(SingleQubitGate::Y);
                break;
            case 'Z':
                SGate.push_back(SingleQubitGate::Z);
                break;
            case 'H':
                SGate.push_back(SingleQubitGate::H);
                break;
            case 'N':
                add_gates=false;
                break;
            default:
                std::cout << "Unknown gate '" << gate << "'. Enter X, Y, Z, H, or N.\n";
                break;
        }

    }
}

void prepare_two_qubit_gate_list(){
    std::cout << "Enter two-qubit gates in order:\n"
              << "HF = H on first qubit, HS = H on second qubit\n"
              << "CF = CNOT with first qubit as control, "
                 "CS = CNOT with second qubit as control\n"
              << "Enter N to finish.\n";
    bool add_gates=true;
    while(add_gates){
        std::string gate;
        if (!(std::cin >> gate)) {
            std::cout << "Input ended before the gate list was complete.\n";
            return;
        }
        if (gate == "HF") {
        TGate.push_back(TwoQubitGate::HFirst);
        } else if (gate == "HS") {
        TGate.push_back(TwoQubitGate::HSecond);
        } else if (gate == "CF") {
        TGate.push_back(TwoQubitGate::CnotFirstControl);
        } else if (gate == "CS") {
        TGate.push_back(TwoQubitGate::CnotSecondControl);
        } else if (gate == "N") {
        add_gates = false;
        } else {
        std::cout << "Unknown gate '" << gate << "'. Enter HF, HS, CF, CS, or N.\n";
        }
    }

}

void execute_qubit_gates(){
    if(SGate.empty()){
        return;
    }
    for(auto x:SGate){
        switch(x){
            case SingleQubitGate::H:
            q.applyhadamard();
            break;
            case SingleQubitGate::X:
            q.applyx();
            break;
            case SingleQubitGate::Y:
            q.applyy();
            break;
            case SingleQubitGate::Z:
            q.applyz();
            break;
        }
    }
}

void execute_twoqubit_gates(){
    if(TGate.empty()){
        return;
    }
    for(auto x:TGate){
        switch(x){
            case TwoQubitGate::HFirst:
            tq.applyHadamardToFirst();
            break;
            case TwoQubitGate::HSecond:
            tq.applyHadamardToSecond();
            break;
            case TwoQubitGate::CnotFirstControl:
            tq.applyCNOTWithFirstAsControl();
            break;
            case TwoQubitGate::CnotSecondControl:
            tq.applyCNOTWithSecondAsControl();
            break;
        }
    }
}
void print_single_qubit(){
    q.printState();
}

void print_two_qubit(){
    tq.printState();
}
};

int main(){
    std::cout << "Choose simulator mode: 1 for one qubit, 2 for two qubits.\n";
    int mode;
    if (!(std::cin >> mode)) {
        std::cerr << "Invalid input. Enter 1 or 2.\n";
        return 1;
    }
    execution instance;
    switch(mode){
        case 1:
          instance.prepare_single_qubit_gate_list();
          instance.execute_qubit_gates();
          instance.print_single_qubit();
          break;
        case 2:
          instance.prepare_two_qubit_gate_list();
          instance.execute_twoqubit_gates();
          instance.print_two_qubit();
          break;
        default:
          std::cerr << "Unknown mode. Enter 1 or 2.\n";
          return 1;
        }
}