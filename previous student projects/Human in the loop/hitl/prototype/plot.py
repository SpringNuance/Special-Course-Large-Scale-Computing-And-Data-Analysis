import pandas
import matplotlib.pyplot as plt


if __name__ == '__main__':
    results = pandas.read_csv('../results/results.csv')
    plt.scatter(results['hyperparameter'], results['result'])
    plt.title('Training Accuracy with different hyperparameter')
    plt.xlabel('hyperparameter')
    plt.ylabel('result')
    plt.show()
