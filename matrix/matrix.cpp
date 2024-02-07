#include <thread>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <time.h>

template<typename NumT>
class Matrix {
public:
    Matrix() : mItems(nullptr), mRows(0), mCols(0)
    {}
    explicit Matrix(size_t rows, size_t cols, bool fill_random = true) {
        if (!rows || !cols) {
            mItems = nullptr;
            mRows = 0;
            mCols = 0;
            return;
        }

        mItems = new NumT[cols * rows];
        mRows = rows;
        mCols = cols;

        if (!fill_random)
            return;

        for (size_t i = 0; i < mRows * mCols; ++i) {
            mItems[i] = (float)rand() / RAND_MAX * 10;
        }
    }
    Matrix(const Matrix& other) {
        if (!other.mItems) {
            mItems = nullptr;
            mRows = 0;
            mCols = 0;
            return;
        }

        mItems = new NumT[other.mCols * other.mRows];
        mRows = other.mRows;
        mCols = other.mCols;

        memcpy(mItems, other.mItems, sizeof(NumT) * mCols * mRows);
    }

    void clear() {
        if (mItems) delete[] mItems;
        mItems = 0;
        mRows = 0;
        mCols = 0;
    }

    ~Matrix() {
        if (mItems) delete[] mItems;
    }

    Matrix& operator =(const Matrix& other) {
        if (this == &other)
            return *this;

        clear();

        if (!other.mItems) {
            return *this;
        }

        mItems = new NumT[other.mCols * other.mRows];
        mRows = other.mRows;
        mCols = other.mCols;

        memcpy(mItems, other.mItems, sizeof(NumT) * mCols * mRows);

        return *this;
    }

    size_t rows() const {
        return mRows;
    }
    size_t cols() const {
        return mCols;
    }

    NumT& at(size_t i, size_t j) const {
        if (i >= mRows || j >= mCols)
            throw std::out_of_range("invalid indeces to access at matrix");

        size_t index = i * mCols + j;
        return mItems[index];
    }
private:
    NumT* mItems;
    size_t mRows;
    size_t mCols;
};

template<typename NumT>
Matrix<NumT> matrix_mul(const Matrix<NumT>& A, const Matrix<NumT>& B) {
    if (!A.rows() || !B.rows()) {
        throw std::invalid_argument("can't multiply empty matrices");
    }
    if (A.cols() != B.rows())
        throw std::invalid_argument("invalid matrix sizes for multiplication");

    Matrix<NumT> res(A.rows(), B.cols(), false);
    for (size_t i = 0; i < A.rows(); ++i) {
        for (size_t j = 0; j < B.cols(); ++j) {
            res.at(i, j) = 0;
            for (size_t k = 0; k < A.rows(); ++k) {
                res.at(i, j) += A.at(i, k) * B.at(k, j);
            }
        }
    }

    return res;
}

template<typename NumT>
void matrix_print(const Matrix<NumT>& matrix) {
    for (size_t i = 0; i < mRows; ++i) {
        for (size_t j = 0; j < mCols; ++j) {
            std::cout << std::setw(4) << matrix.at(i, j);
        }
        std::cout << "\n";
    }
}

int main()
{   
    size_t n = 100;
    Matrix<int8_t> A(n, n);
    Matrix<int8_t> B(n, n);
    std::cout << "initialized" << std::endl;

    // A.print();
    // std::cout << "\n";
    
    // B.print();
    // std::cout << "\n";
    clock_t t1 = clock();
    auto C = matrix_mul(A, B);
    clock_t t2 = clock();

    // C.print();
    std::cout << "multiplied in " << 1000.0 * (t2 - t1) / CLOCKS_PER_SEC << "ms" << std::endl;
}

