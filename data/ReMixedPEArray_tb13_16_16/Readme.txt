    LayerDimension layer1(56,56,64);
    LayerDimension layer2(56,56,64);

    equivalent workload (Mop): 220.5
    group size : 16

    
/// for network sparsity
#define KERNEL_SPARSE_RATE 0
#define FEATURE_ZERO_PERCENT 0
/// for value precision
#define KERNEL_16_BIT_RATE 100    ///  KERNEL_16_BIT_RATE%    of the non-zero weight     is 16 bit
#define FEATURE_16_BIT_PERCENT 100 /// FEATURE_16_BIT_PERCENT% of the non-zero activation is 16 bit