    LayerDimension layer1(24,24,16);
    LayerDimension layer2(24,24,16);

    equivalent workload (Mop): 2.53125
    group size : 16

    
/// for network sparsity
#define KERNEL_SPARSE_RATE 30
#define FEATURE_ZERO_PERCENT 30
/// for value precision
#define KERNEL_16_BIT_RATE 0    ///  KERNEL_16_BIT_RATE%    of the non-zero weight     is 16 bit
#define FEATURE_16_BIT_PERCENT 0 /// FEATURE_16_BIT_PERCENT% of the non-zero activation is 16 bit