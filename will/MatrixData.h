#pragma once
#include "CudaControl.h"
#include "types.h"
#include <memory>

namespace cccc
{

//����ڶ��豸���ܣ�������ͬʱ�������豸ָ�룬���Թ���ָ��ʹ�ô���
struct MatrixData
{
    CudaControl* cuda_ = nullptr;    //Ϊ�ձ�ʾ��cpu�У�Ҳ��Ĭ�����
    real* data_ = nullptr;
    int64_t occupy_data_size_ = 0;    //ʵ��ռ�õ����ݳ��ȣ��������óߴ粻���ڴ�ֵ��ʱ�򲻻����·����ڴ�
public:
    MatrixData() = default;
    ~MatrixData() { free(); }
    MatrixData(const MatrixData&) = delete;
    MatrixData& operator=(const MatrixData) = delete;
    void setCuda(CudaControl* cuda) { cuda_ = cuda; }
    void setCudaAsCurrent() { cuda_ = CudaControl::getCurrentCuda(); }
    real* resize(int64_t size, bool reserve_data = true, bool force = false);    //resizeǰӦsetCuda
    std::shared_ptr<real*> make_shared_data() { return std::make_shared<real*>(data_); }
    void free();

public:
    static int64_t copy(DeviceType dt_src, const real* src, DeviceType dt_dst, real* dst, int64_t size)
    {
        if (src == nullptr || dst == nullptr || src == dst)
        {
            return 0;
        }
        int64_t size_in_byte = size * sizeof(real);
        cudaError state = cudaSuccess;
        if (dt_dst == DeviceType::GPU && dt_src == DeviceType::GPU)
        {
            state = cudaMemcpy(dst, src, size_in_byte, cudaMemcpyDeviceToDevice);
        }
        else if (dt_dst == DeviceType::GPU && dt_src == DeviceType::CPU)
        {
            state = cudaMemcpy(dst, src, size_in_byte, cudaMemcpyHostToDevice);
        }
        else if (dt_dst == DeviceType::CPU && dt_src == DeviceType::GPU)
        {
            state = cudaMemcpy(dst, src, size_in_byte, cudaMemcpyDeviceToHost);
        }
        else
        {
            memcpy(dst, src, size_in_byte);
        }
        if (state != cudaSuccess)
        {
            fmt::print(stderr, "Error: cudaMemcpy failed with error code is {}, size in byte is {} ({:g})!\n", state, size_in_byte, 1.0 * size_in_byte);
        }
        return size;
    }
};

}    // namespace cccc