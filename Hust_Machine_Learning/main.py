import numpy as np
import pandas as pd


def logicSpilt(data, i, n):
    br = int(n / 10)
    a = data[br * (i - 1):br * i]
    b = data[0:br * (i - 1)]
    c = data[br * i:n]
    d = pd.concat([b, c], axis=0)
    return a, d


def logicStandardize(x, mean, std):
    # 通过减去均值并除以标准差的方式进行标准化
    return (x - mean) / std


def logicLoadData(xfile, yfile, num):
    # 读取文件，转化为矩阵输出
    data = pd.read_csv(xfile, header=None)
    target = pd.read_csv(yfile, header=None)
    m = data.shape[0]
    test_data, train_data = logicSpilt(data, num, m)
    test_target, train_target = logicSpilt(target, num, m)
    std = np.sqrt(np.var(train_data, axis=0, ddof=1).tolist())  # 获取标准差
    mean = np.mean(train_data, axis=0).tolist()
    train_data = train_data.apply(logicStandardize, args=(mean, std), axis=1)
    test_data = test_data.apply(logicStandardize, args=(mean, std), axis=1)
    return np.mat(test_data), np.mat(train_data), np.mat(test_target), np.mat(
        train_target)


def cbindOne(dataArr):
    # dataArr为数组/矩阵，输出矩阵
    dataMat = np.mat(dataArr)
    n = dataMat.shape[0]
    dataFirstColumn = np.mat(np.ones((n, 1)))
    dataMatrix = np.append(dataFirstColumn, dataMat, axis=1)
    return dataMatrix


def logisticVal(coeff, dataArr):
    # coeff为列向量，dataArr为数组/矩阵，输出矩阵
    dataMat = np.mat(dataArr)
    n = dataMat.shape[0]
    hatVal = np.mat(np.zeros((n, 1)))
    for i in range(n):
        z = dataMat[i] * np.mat(coeff)
        hatVal[i][0] = 1 / (1 + np.exp(-z))
    return hatVal


def bulidLogistics(dataArr, classLabels, weight):
    # dataArr为加上第一列的矩阵，weight为数组
    labelMat = np.mat(classLabels)
    dataMat = np.mat(dataArr)
    n, m = dataMat.shape
    learningRate = 1.0
    coeff = np.mat(np.zeros((m, 1)))
    weightMat = np.diag(weight.tolist())
    for i in range(60):
        hatVal = logisticVal(coeff, dataMat)
        gradient = dataMat.T * (weightMat * (hatVal - labelMat))
        coeff = coeff - learningRate * gradient
        if np.linalg.norm(gradient.A) < 5 * 10e-3:
            break
    return coeff


def classifyLogic(hatVal):
    n = hatVal.shape[0]
    resultArr = np.mat(np.ones((n, 1)))
    for i in range(n):
        if hatVal[i][0] < 0.5:
            resultArr[i][0] = 0
    return resultArr


def adaTrainLogic(dataArr, classLabels, num):
    # dataArr为加上第一列的矩阵，num为迭代次数
    coefficient = []
    dataMat = np.mat(dataArr)
    labelMat = np.mat(classLabels)
    n = dataMat.shape[0]
    weight = np.ones(n) / n
    alphaList = [0] * num
    for i in range(int(num)):
        errorArr = np.mat(np.ones((n, 1)))
        coeff = bulidLogistics(dataMat, labelMat, weight)
        coefficient.append(coeff)
        hatVal = logisticVal(coeff, dataMat)
        predictedVal = classifyLogic(hatVal)
        errorArr[predictedVal == labelMat] = 0
        error = np.mat(weight) * errorArr
        if error > 0.5:
            break
        else:
            alpha = 0.5 * np.log((1 - error) / max(error, 10e-16) + 10e-16)
            alphaList[i] = alpha
        for j in range(n):
            if labelMat[j][0] == predictedVal[j][0]:
                weight[j] = weight[j] * np.exp(-alpha)
            else:
                weight[j] = weight[j] * np.exp(alpha)
        weight = weight / weight.sum()
    return coefficient, alphaList


def adaClassifyLogic(dataMat, coefficient, alphaList, num):
    m = np.shape(dataMat)[0]
    aggClass = np.mat(np.zeros((m, 1)))
    a = np.array(alphaList, dtype=object)
    k = np.where(a == 0)[0]
    if len(k) > 0:
        num = k[0]
    for i in range(int(num)):
        coeff = coefficient[i]
        alpha = alphaList[i]
        resultArr = classifyLogic(logisticVal(coeff, dataMat))
        resultArr[resultArr == 0] = -1
        aggClass += resultArr * alpha
    aggClass[aggClass > 0] = 1
    aggClass[aggClass <= 0] = 0
    return aggClass


def deciSpilt(data, i, n):
    br = int(n / 10)
    a = data[br * (i - 1):br * i, :]
    b = data[0:br * (i - 1), :]
    c = data[br * i:n, :]
    d = np.concatenate([b, c], axis=0)
    return a, d


def deciLoadData(xfile, yfile):
    data = pd.read_csv(xfile, header=None)
    target = pd.read_csv(yfile, header=None)
    datMat = np.mat(data)
    classLabels = np.mat(target)
    return datMat, classLabels


def stumpClassify(dataMat, dimen, threshVal, direction):
    resultArr = np.ones([dataMat.shape[0], 1])
    if direction == "lt":
        resultArr[dataMat[:, dimen] <= threshVal] = 0
    else:
        resultArr[dataMat[:, dimen] >= threshVal] = 0
    return resultArr


def bulidStump(dataMat, classLabels, weight):
    # 创建决策树桩，dataArr为数据数组/矩阵，classLabels为目标数组/矩阵，weight为权重矩阵
    dataMat = np.mat(dataMat)
    labelMat = np.mat(classLabels)
    m, n = dataMat.shape
    numsteps = 30
    minError = np.Inf
    bestStump = {}
    bestClass = np.ones((m, 1))
    for i in range(n):
        rangeMax = dataMat[:, i].max()
        rangeMin = dataMat[:, i].min()
        stepSize = (rangeMax - rangeMin) / numsteps
        for j in range(-1, int(numsteps) + 1):
            threshVal = rangeMin + j * stepSize
            for direction in ["lt", "gt"]:
                predictedVal = stumpClassify(dataMat, i, threshVal, direction)
                errorArr = np.mat(np.ones((m, 1)))
                errorArr[predictedVal == labelMat] = 0
                weightedErr = weight.T * errorArr
                if weightedErr < minError:
                    minError = weightedErr
                    bestClass = predictedVal.copy()
                    bestStump["dimen"] = i
                    bestStump["threshVal"] = threshVal
                    bestStump["direction"] = direction
    return bestStump, minError, bestClass


def adaTrainStump(dataMat, classLabels, num):
    weakClassArr = []
    labelMat = np.mat(classLabels)
    m = dataMat.shape[0]
    weight = np.mat(np.ones((m, 1))) / m
    for i in range(int(num)):
        bestStump, error, bestClass = bulidStump(dataMat, classLabels, weight)
        if error > 0.5:
            bestStump['alpha'] = 0
        alpha = 0.5 * np.log((1 - error) / max(error, 10e-16) + 10e-16)
        bestStump['alpha'] = alpha
        weakClassArr.append(bestStump)
        for j in range(m):
            if labelMat[j][0] == bestClass[j][0]:
                weight[j][0] = weight[j][0] * np.exp(-alpha)
            else:
                weight[j][0] = weight[j][0] * np.exp(alpha)
        weight = weight / weight.sum()
    return weakClassArr


def adaClassifyStump(dataMat, weakClassArr, num):
    m = np.shape(dataMat)[0]
    aggClass = np.mat(np.zeros((m, 1)))
    for i in range(int(num)):
        resultArr = stumpClassify(dataMat, weakClassArr[i]["dimen"],
                                  weakClassArr[i]["threshVal"], weakClassArr[i]["direction"])
        for j in range(m):
            if resultArr[j][0] == 0:
                resultArr[j][0] = -1
        aggClass += resultArr * weakClassArr[i]["alpha"]
    for j in range(m):
        if aggClass[j][0] > 0:
            aggClass[j][0] = 1
        else:
            aggClass[j][0] = 0
    return aggClass


class Adaboost():

    def __init__(self, base):
        '''
        :param base: 基分类器编号 0 代表对数几率回归 1 代表决策树桩
        在此函数中对模型相关参数进行初始化，如后续还需要使用参数请赋初始值
        '''
        self.base = base
        self.model = 0

    def fit(self, x_file, y_file):
        '''
        在此函数中训练模型
        :param x_file:训练数据(data.csv)
        :param y_file:训练数据标记(targets.csv)
        '''
        self.train_data = x_file
        self.train_target = y_file
        if self.base == 0:
            for num in [1, 5, 10, 100]:
                for i in range(1, 11):
                    test_data, train_data, test_target, train_target = logicLoadData(
                        x_file, y_file, i)
                    coefficient, alphaList = adaTrainLogic(
                        train_data, train_target, num)
                    resultMat = adaClassifyLogic(test_data, coefficient,
                                                 alphaList, num)
                    resultDF = pd.DataFrame(resultMat)
                    step = len(resultDF)
                    resultDF.index = range(step * (i - 1) + 1, step * i + 1)
                    resultDF.to_csv('experiments/base%d_fold%d.csv' % (num, i),
                                    header=False)
        if self.base == 1:
            for num in [1, 5, 10, 100]:
                for i in range(1, 11):
                    dataMat, classLabels = deciLoadData(x_file, y_file)
                    m, n = np.shape(dataMat)
                    test_data, train_data = deciSpilt(dataMat, i, m)
                    test_target, train_target = deciSpilt(classLabels, i, m)
                    m = train_data.shape[0]
                    weakClassifyArr = adaTrainStump(train_data, train_target,
                                                    num)
                    resultMat = adaClassifyStump(test_data, weakClassifyArr,
                                                 num)
                    resultDF = pd.DataFrame(resultMat)
                    step = len(resultDF)
                    resultDF.index = range(step * (i - 1) + 1, step * i + 1)
                    resultDF.to_csv('experiments/base%d_fold%d.csv' % (num, i),
                                    header=False)

    def predict(self, x_file):
        '''
        :param x_file:测试集文件夹(后缀为csv)
        :return: 训练模型对测试集的预测标记
        '''
        test_data = pd.read_csv(x_file, header=None)
        train_data = pd.read_csv(self.train_data, header=None)
        train_target = pd.read_csv(self.train_target, header=None)
        if self.base == 0:
            std = np.sqrt(np.var(train_data, axis=0, ddof=1).tolist())  # 获取标准差
            mean = np.mean(train_data, axis=0).tolist()
            test_data = test_data.apply(logicStandardize, args=(mean, std), axis=1)
            train_data = train_data.apply(logicStandardize, args=(mean, std), axis=1)
            train_data = cbindOne(train_data)
            dataMat = cbindOne(np.mat(test_data))
            coefficient, alphaList = adaTrainLogic(train_data, train_target, 100)
            resultMat = adaClassifyLogic(dataMat, coefficient, alphaList, 100)
            return resultMat
        if self.base == 1:
            dataMat = np.mat(train_data)
            weakClassifyArr = adaTrainStump(train_data, train_target, 100)
            resultMat = adaClassifyStump(np.mat(test_data), weakClassifyArr, 100)
            return resultMat


""" test1 = Adaboost(base=0)
test1.fit("data1.csv", "targets1.csv") """
