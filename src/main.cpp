#include <iostream>
#include <complex>
#include <cmath>
#include <utility> 
#include <random>
#include <vector>
#include <string>
#include <limits>
#include <stdexcept>

enum class SingleQubitGate { X, Y, Z, H };
enum class TwoQubitGate { HFirst, HSecond, CnotFirstControl, CnotSecondControl };

enum class GateType { X, Y, Z, H, CNOT };

struct GateOperation {
    GateType type;
    std::size_t targetQubit;
    std::size_t controlQubit = 0;
};

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

class QuantumState {
public:

    //we get a n bit state and it can be represented by 2^n values
    //the 2^n vector stores those amplitudes
    explicit QuantumState(std::size_t qubitCount)
        : qubitCount(qubitCount),
          amplitudes(stateVectorSize(qubitCount))
    {
        amplitudes[0] = {1.0, 0.0};
    }

    QuantumState(
    std::size_t qubitCount,
    std::vector<std::complex<double>> initialAmplitudes)
        : qubitCount(qubitCount),
          amplitudes(std::move(initialAmplitudes))
    {
        if (amplitudes.size() != stateVectorSize(qubitCount)) {
            throw std::invalid_argument("Amplitude count must be 2^qubitCount.");
        }
        if (!isNormalized()) {
            throw std::invalid_argument("Initial amplitudes must be normalized.");
        }
    }

    void printState() const
    {
        for (std::size_t index = 0; index < amplitudes.size(); ++index) {
            std::cout << "index " << index
                  << ": amplitude " << amplitudes[index]
                  << ", probability " << std::norm(amplitudes[index])
                  << '\n';
        }
    }

    bool isNormalized() const {
        double totalProbability = 0.0;

        for (const auto& amplitude : amplitudes) {
            totalProbability += std::norm(amplitude);
        }

        return std::abs(totalProbability - 1.0) < 1e-10;
    }

    void applyHadamard(std::size_t targetQubit)
{
    if (targetQubit >= qubitCount) {
        throw std::invalid_argument("Invalid target qubit.");
    }

    // Find which bit represents the target qubit.
    std::size_t mask = 1ULL << (qubitCount - 1 - targetQubit);

    const double invSqrt2 = 1.0 / std::sqrt(2.0);

    for (std::size_t i = 0; i < amplitudes.size(); ++i) {

        // Process only indices whose target bit is zero.
        if ((i & mask) == 0) {

            // Find the partner index whose target bit is one.
            std::size_t j = i | mask;

            // Save the original amplitudes before changing either.
            auto a = amplitudes[i];
            auto b = amplitudes[j];

            // Apply the Hadamard transformation.
            amplitudes[i] = (a + b) * invSqrt2;
            amplitudes[j] = (a - b) * invSqrt2;
        }
    }
}

    void applyx(std::size_t targetQubit)
{
    if (targetQubit >= qubitCount) {
        throw std::invalid_argument("Invalid target qubit.");
    }

    // Find which bit represents the target qubit.
    std::size_t mask = 1ULL << (qubitCount - 1 - targetQubit);

    for (std::size_t i = 0; i < amplitudes.size(); ++i) {

        // Process only indices whose target bit is zero.
        if ((i & mask) == 0) {

            // Find the partner index whose target bit is one.
            std::size_t j = i | mask;

            // Apply the x gate
            std::swap(amplitudes[i],amplitudes[j]);
        }
    }
}

    void applyz(std::size_t targetQubit)
{
    if (targetQubit >= qubitCount) {
        throw std::invalid_argument("Invalid target qubit.");
    }

    // Find which bit represents the target qubit.
    std::size_t mask = 1ULL << (qubitCount - 1 - targetQubit);

    for (std::size_t i = 0; i < amplitudes.size(); ++i) {

        // Process only indices whose target bit is zero.
        if ((i & mask) == 0) {

            // Find the partner index whose target bit is one.
            std::size_t j = i | mask;

            // Apply the z gate
            amplitudes[j]=-amplitudes[j];
        }
    }
}

    void applyy(std::size_t targetQubit)
{
    if (targetQubit >= qubitCount) {
        throw std::invalid_argument("Invalid target qubit.");
    }

    // Find which bit represents the target qubit.
    std::size_t mask = 1ULL << (qubitCount - 1 - targetQubit);

    for (std::size_t i = 0; i < amplitudes.size(); ++i) {

        // Process only indices whose target bit is zero.
        if ((i & mask) == 0) {

            // Find the partner index whose target bit is one.
            std::size_t j = i | mask;

            auto iota = std::complex<double>{0.0, 1.0};
            // Apply the y gate
            auto temp = amplitudes[i];
            amplitudes[i] = -iota*amplitudes[j];
            amplitudes[j] = iota*temp;
        }
    }
}

void applyCNOT(std::size_t controlQubit,
               std::size_t targetQubit)
{
    if (controlQubit >= qubitCount ||
        targetQubit >= qubitCount ||
        controlQubit == targetQubit) {
        throw std::invalid_argument("Invalid CNOT qubits.");
    }

    // Find the bit positions for control and target.
    std::size_t controlMask =
        1ULL << (qubitCount - 1 - controlQubit);

    std::size_t targetMask =
        1ULL << (qubitCount - 1 - targetQubit);

    for (std::size_t i = 0; i < amplitudes.size(); ++i) {

        // Control must be 1 and target must be 0.
        if ((i & controlMask) != 0 &&
            (i & targetMask) == 0) {

            // Flip the target bit to find the partner.
            std::size_t j = i | targetMask;

            // CNOT swaps the two amplitudes.
            std::swap(amplitudes[i], amplitudes[j]);
        }
    }
}

    void executeCircuit(const std::vector<GateOperation>& circuit)
    {
        for (const auto& operation : circuit) {
            switch (operation.type) {
            case GateType::X:
                applyx(operation.targetQubit);
                break;
            case GateType::Y:
                applyy(operation.targetQubit);
                break;
            case GateType::Z:
                applyz(operation.targetQubit);
                break;
            case GateType::H:
                applyHadamard(operation.targetQubit);
                break;
            case GateType::CNOT:
                applyCNOT(operation.controlQubit, operation.targetQubit);
                break;
            default:
                throw std::invalid_argument("Unknown gate type in circuit.");
            }
        }
    }

    std::size_t measure(std::mt19937& engine)
    {
        double totalProbability = 0.0;
        for (const auto& amplitude : amplitudes) {
            totalProbability += std::norm(amplitude);
        }
        if (!std::isfinite(totalProbability) || totalProbability <= 0.0) {
            throw std::logic_error("Cannot measure a state with invalid total probability.");
        }

        const double random =
            std::uniform_real_distribution<double>(0.0, totalProbability)(engine);
        double cumulativeProbability = 0.0;
        std::size_t outcome = amplitudes.size();

        for (std::size_t index = 0; index < amplitudes.size(); ++index) {
            cumulativeProbability += std::norm(amplitudes[index]);
            if (random < cumulativeProbability) {
                outcome = index;
                break;
            }
        }
        if (outcome == amplitudes.size()) {
            throw std::logic_error("Failed to select a measurement outcome.");
        }

        for (std::size_t index = 0; index < amplitudes.size(); ++index) {
            amplitudes[index] = {index == outcome ? 1.0 : 0.0, 0.0};
        }
        return outcome;
    }

private:
    static std::size_t stateVectorSize(std::size_t qubitCount)
    {
        if (qubitCount >= std::numeric_limits<std::size_t>::digits) {
            throw std::length_error("Qubit count is too large for state-vector indexing.");
        }
        return std::size_t{1} << qubitCount;
    }

    std::size_t qubitCount;
    std::vector<std::complex<double>> amplitudes;
};

bool readQubitIndex(const char* prompt, std::size_t qubitCount, std::size_t& index)
{
    std::cout << prompt;
    long long inputIndex;
    if (!(std::cin >> inputIndex)) {
        std::cerr << "Invalid input: expected a qubit index.\n";
        return false;
    }
    if (inputIndex < 0 ||
        static_cast<unsigned long long>(inputIndex) >= qubitCount) {
        std::cerr << "Qubit index must be between 0 and "
                  << (qubitCount - 1) << ".\n";
        return false;
    }
    index = static_cast<std::size_t>(inputIndex);
    return true;
}

int main()
{
    std::cout << "Number of qubits (indices start at 0; qubit 0 is leftmost): ";
    long long requestedQubitCount;
    if (!(std::cin >> requestedQubitCount) || requestedQubitCount <= 0 ||
        static_cast<unsigned long long>(requestedQubitCount) >=
            std::numeric_limits<std::size_t>::digits) {
        std::cerr << "Enter a positive qubit count supported by this platform.\n";
        return 1;
    }
    const auto qubitCount = static_cast<std::size_t>(requestedQubitCount);

    std::cout << "Number of gates: ";
    long long requestedGateCount;
    if (!(std::cin >> requestedGateCount) || requestedGateCount < 0 ||
        static_cast<unsigned long long>(requestedGateCount) >
            std::numeric_limits<std::size_t>::max()) {
        std::cerr << "Enter a non-negative gate count.\n";
        return 1;
    }

    std::vector<GateOperation> circuit;
    circuit.reserve(static_cast<std::size_t>(requestedGateCount));

    for (long long gateIndex = 0; gateIndex < requestedGateCount; ++gateIndex) {
        std::cout << "Gate " << (gateIndex + 1)
                  << " (X, Y, Z, H, CNOT): ";
        std::string gateName;
        if (!(std::cin >> gateName)) {
            std::cerr << "Input ended before all gates were entered.\n";
            return 1;
        }

        GateOperation operation{};
        if (gateName == "CNOT") {
            operation.type = GateType::CNOT;
            if (!readQubitIndex("Control qubit: ", qubitCount, operation.controlQubit) ||
                !readQubitIndex("Target qubit: ", qubitCount, operation.targetQubit)) {
                return 1;
            }
            if (operation.controlQubit == operation.targetQubit) {
                std::cerr << "CNOT control and target must be different qubits.\n";
                return 1;
            }
        } else {
            if (gateName == "X") {
                operation.type = GateType::X;
            } else if (gateName == "Y") {
                operation.type = GateType::Y;
            } else if (gateName == "Z") {
                operation.type = GateType::Z;
            } else if (gateName == "H") {
                operation.type = GateType::H;
            } else {
                std::cerr << "Unknown gate '" << gateName
                          << "'. Enter X, Y, Z, H, or CNOT.\n";
                return 1;
            }

            if (!readQubitIndex("Target qubit: ", qubitCount, operation.targetQubit)) {
                return 1;
            }
        }
        circuit.push_back(operation);
    }

    QuantumState state{qubitCount};
    state.executeCircuit(circuit);
    std::cout << "Resulting state:\n";
    state.printState();
    std::cout << "Normalized: " << (state.isNormalized() ? "Yes" : "No") << '\n';
}