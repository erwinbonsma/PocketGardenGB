# Generate look-up values used by CellCountHistory.cpp
print(
    ", ".join("{:4d}".format(
        max(0, 4 * i + i * (i + 1) // 2 - 2)
    ) for i in range(64))
)