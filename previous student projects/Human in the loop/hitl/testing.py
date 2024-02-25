import time


# Another blackbox function which takes some time to run
def test(a, b, c):
    time.sleep(5)
    result = {
        'accuracy': (b + c) / (a + b + c)
    }

    return result
