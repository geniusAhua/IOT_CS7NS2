function quickSortInPlace(arr, left = 0, right = arr.length - 1) {
    if (left >= right) {
        return;
    }
    var pivotIndex = partition(arr, left, right);
    quickSortInPlace(arr, left, pivotIndex - 1);
    quickSortInPlace(arr, pivotIndex + 1, right);
}

function partition(arr, left, right) {
    var pivotIndex = left;
    var pivot = arr[pivotIndex];
    for (var i = left + 1; i <= right; i++) {
        if (arr[i] < pivot) {
            pivotIndex++;
            swap(arr, i, pivotIndex);
        }
    }
    swap(arr, left, pivotIndex);
    return pivotIndex;
}

function swap(arr, i, j) {
    var temp = arr[i];
    arr[i] = arr[j];
    arr[j] = temp;
}
quickSortInPlace([1,2,6,2,3,5,7,2,1])
console.log(arr)