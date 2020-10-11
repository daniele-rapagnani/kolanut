let Array = {
    find = |arr, obj| => {
        if (#arr == 0) {
            return null;
        }

        for (let i in 0..#arr) {
            if (arr[i] == obj) {
                return i;
            }
        }

        return null;
    },

    delete = |arr, obj| => {
        let idx = null;

        if (types.isInteger(obj)) {
            idx = obj;
        } else {
            idx = Array.find(arr, obj);
        }

        if (idx == null) {
            return false;
        }

        return array.delete(arr, idx);
    }
};

return Array;