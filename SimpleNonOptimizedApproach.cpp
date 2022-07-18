#include <iostream>
#include <vector>
#include <string>
#include <fstream>

/*
 * This code is a simple, but very inefficient code that can produce an .stl file from a structured triad. The inefficiency
 * may be attributed to the fact that the code defines two triangular faces for every external face of all grid cells.
 * The code was defined as a first try for a larger project in which CFD simulations will be done on a Solid-Fluid domain.
 * The Domain thus only takes zero or unity values where unity may indicate a solid cell and zero a fluid cell, or vice versa.
 *
 * For visualization a randomized input was used, this can be removed such that the nested for-loops only contain
 * DummyDomain[i+1][j+1][k+1] = Domain[i][j][k];
 */

int AbsSumVector(std::vector<int> &vector);
std::vector<int> AbsReverseNormal(std::vector<int> &vector);
std::string SortNormal(std::vector<int> &vector, int sum, double GridSize, int &i, int &j, int &k);

int main() {
    int Nx{6};
    int Ny{6};
    int Nz{6};
    double h{0.01};

    std::vector<std::vector<std::vector<int>>> Domain (Nx,std::vector<std::vector<int>>(Ny,std::vector<int>(Nz,1)));

    // Define dummy domain
    std::vector<std::vector<std::vector<int>>> DummyDomain (Nx+2,std::vector<std::vector<int>>(Ny+2,std::vector<int>(Nz+2,0)));
    // Fill dummy domain
    for (int i{0}; i<Nx; i++)
    {
        for (int j{0}; j<Ny; j++)
        {
            for (int k{0}; k< Nz; k++)
            {
                DummyDomain[i+1][j+1][k+1] = Domain[i][j][k] *  static_cast<int>(std::rand()*1.9 / RAND_MAX);
            }
        }
    }

    // Open file and write face data
    std::ofstream DataFile("../model.stl");
    DataFile << "solid model\n";
    for (int i{1}; i<Nx+2; i++)
    {
        for (int j{1}; j<Ny+2; j++)
        {
            for (int k{1}; k< Nz+2; k++)
            {
                // Define face data for every cell
                int CellValue{DummyDomain[i][j][k]};
                std::vector<int> normal {(DummyDomain[i-1][j][k] - CellValue), (DummyDomain[i][j-1][k] - CellValue), (DummyDomain[i][j][k-1] - CellValue)};
                int sum{AbsSumVector(normal)};
                DataFile << SortNormal(normal, sum, h, i, j, k);
            }
        }
    }
    DataFile << "endsolid model\n";
    return 0;
}

int AbsSumVector(std::vector<int> &vector)
{   // Return absolute sum of vector entries
    unsigned long long N = vector.size();
    int sum{0};
    for (int i{0}; i<N;i++)
    {
        sum += abs(vector[i]);
    }
    return sum;
}

std::string SortNormal(std::vector<int> &vector, int sum, double GridSize, int &i, int &j, int &k)
{   // Sort the normal vector by absolute sum and return .stl format external faces
    if (sum > 1)
    {
        std::vector<int> vecA{vector[0], 0, 0};
        int sumA{AbsSumVector(vecA)};
        std::string A = SortNormal(vecA, sumA, GridSize, i, j, k);

        std::vector<int> vecB{0, vector[1], 0};
        int sumB{AbsSumVector(vecB)};
        std::string B = SortNormal(vecB, sumB, GridSize, i, j, k);

        std::vector<int> vecC{0, 0, vector[2]};
        int sumC{AbsSumVector(vecC)};
        std::string C = SortNormal(vecC, sumC, GridSize, i, j, k);

        return A + B + C;
    }
    else if (sum == 1)
    {
        std::string A = "facet normal " + std::to_string(vector[0]) + " " + std::to_string(vector[1]) + " " + std::to_string(vector[2]) + "\n";
        std::string B = "outer loop\n";
        std::string C = "vertex " + std::to_string(i*GridSize) + " " + std::to_string(j*GridSize) + " " + std::to_string(k*GridSize) + "\n";
        std::vector<int> ReversedNormal{AbsReverseNormal(vector)};
        std::string D = "vertex " + std::to_string((i+ReversedNormal[0])*GridSize) + " " + std::to_string((j+ReversedNormal[1])*GridSize) + " " + std::to_string((k+ReversedNormal[2])*GridSize) + "\n";
        std::string pre = A + B + C + D;
        std::string post = "endloop\nendfacet\n";
        if (ReversedNormal[0] == 0)
        {
            std::string E1 = "vertex " + std::to_string(i*GridSize) + " " + std::to_string((j+1)*GridSize) + " " + std::to_string(k*GridSize) + "\n";
            std::string E2 = "vertex " + std::to_string(i*GridSize) + " " + std::to_string(j*GridSize) + " " + std::to_string((k+1)*GridSize) + "\n";
            return pre + E1 + post + pre + E2 + post;
        }
        else if (ReversedNormal[1] == 0)
        {
            std::string E1 = "vertex " + std::to_string((i+1)*GridSize) + " " + std::to_string(j*GridSize) + " " + std::to_string(k*GridSize) + "\n";
            std::string E2 = "vertex " + std::to_string(i*GridSize) + " " + std::to_string(j*GridSize) + " " + std::to_string((k+1)*GridSize) + "\n";
            return pre + E1 + post + pre + E2 + post;
        }
        else if (ReversedNormal[2] == 0)
        {
            std::string E1 = "vertex " + std::to_string((i+1)*GridSize) + " " + std::to_string(j*GridSize) + " " + std::to_string(k*GridSize) + "\n";
            std::string E2 = "vertex " + std::to_string(i*GridSize) + " " + std::to_string((j+1)*GridSize) + " " + std::to_string(k*GridSize) + "\n";
            return pre + E1 + post + pre + E2 + post;
        }
    }
    return "";
}

std::vector<int> AbsReverseNormal(std::vector<int> &vector)
{   // Reverse the normal and return the absolute
    unsigned long long N = vector.size();
    std::vector<int> result;
    for (int i{0}; i<N;i++)
    {
        if (vector[i] != 0)
        {
            result.push_back(0);
        }
        else
        {
            result.push_back(1);
        }
    }
    return result;
}
