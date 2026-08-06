#ifndef CP_DATA_STRUCTURES_RANGESUMQUERY_HPP
#define CP_DATA_STRUCTURES_RANGESUMQUERY_HPP

#include <vector>

class RangeSumQuery {
    std::vector<int> Prefix;

public:
    RangeSumQuery(std::vector<int>& nums) {
        Prefix = std::vector<int>(nums.size() + 1);

        for (size_t i = 0; i < nums.size(); i++) {
            Prefix[i+1] = Prefix[i] + nums[i];
        }
    }

    int sumRange(int left, int right) {
        return Prefix[right+1] - Prefix[left];
    }
};

#endif
