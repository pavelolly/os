#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <thread>
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
    Matrix(Matrix&& other) noexcept {
        memcpy(this, &other, sizeof(Matrix));

        other.mItems = nullptr;
        other.mRows = other.mCols = 0;
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

    Matrix& operator =(Matrix&& other) noexcept {
        if (this == &other)
            return *this;

        clear();

        memcpy(this, &other, sizeof(Matrix));

        other.mItems = nullptr;
        other.mRows = other.mCols = 0;

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

    bool operator ==(const Matrix& other) const {
        if (rows() != other.rows() || cols() != other.cols())
            return false;

        for (size_t i = 0; i < rows(); ++i) {
            for (size_t j = 0; j < cols(); ++j) {
                if (at(i, j) != other.at(i, j))
                    return false;
            }
        }

        return true;
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
Matrix<NumT> matrix_mul_threaded(const Matrix<NumT>& A, const Matrix<NumT>& B, size_t number_threads=0) {
    if (!A.rows() || !B.rows()) {
        throw std::invalid_argument("can't multiply empty matrices");
    }
    if (A.cols() != B.rows())
        throw std::invalid_argument("invalid matrix sizes for multiplication");

    if (!number_threads) {
        size_t rows = A.rows();
        size_t max_threads = std::thread::hardware_concurrency();
        number_threads = rows > max_threads ? max_threads : rows;
    }

    Matrix<NumT> res(A.rows(), B.cols(), false);
        
    std::thread* threads = new std::thread[number_threads];
    size_t number_elements = res.rows() * res.cols();
    size_t operations_per_thread = number_elements / number_threads;
    size_t rest_operations = number_elements % number_threads;

    auto multiply = [&res, &A, &B, operations_per_thread, rest_operations](size_t thread_index)
        {
        size_t start, end;
        if (thread_index == 0) {
            start = 0;
            end = operations_per_thread + rest_operations;
        }
        else {
            start = thread_index * operations_per_thread + rest_operations;
            end = (thread_index + 1) * operations_per_thread + rest_operations;
        }

        for (size_t i = start; i < end; ++i) {
            size_t row = i / res.cols();
            size_t col = i % res.cols();

            res.at(row, col) = 0;
            for (size_t k = 0; k < A.rows(); ++k) {
                res.at(row, col) += A.at(row, k) * B.at(k, col);
            }
        }
        };

    for (size_t i = 0; i < number_threads; ++i) {
        threads[i] = std::thread(multiply, i);
    }
    for (size_t i = 0; i < number_threads; ++i) {
        threads[i].join();
    }

    delete[] threads;

    return res;
}

template<typename NumT>
void matrix_print(const Matrix<NumT>& matrix) {
    for (size_t i = 0; i < matrix.rows(); ++i) {
        for (size_t j = 0; j < matrix.cols(); ++j) {
            std::cout << std::setw(4) << matrix.at(i, j);
        }
        std::cout << "\n";
    }
}

int main()
{   
    size_t n = 1000;
    Matrix<int> A(n, n);
    Matrix<int> B(n, n);
    std::cout << "initialized" << std::endl;

    /*matrix_print(A);
    std::cout << "\n";
    
    matrix_print(B);
    std::cout << "\n";*/

    clock_t t1 = clock();
    auto C1 = matrix_mul(A, B);
    clock_t t2 = clock();

    /*matrix_print(C);
    std::cout << "\n";*/
    std::cout << "multiplied with single thread in " << 1000.0 * (t2 - t1) / CLOCKS_PER_SEC << "ms" << std::endl;


    clock_t t3 = clock();
    auto C2 = matrix_mul_threaded(A, B, 12);
    clock_t t4 = clock();

    /*matrix_print(C);
    std::cout << "\n";*/
    std::cout << "multiplied with multiple threads in " << 1000.0 * (t4 - t3) / CLOCKS_PER_SEC << "ms" << std::endl;

    //std::cout << "both results equal? " << std::boolalpha << (C1 == C2) << std::endl;
}

