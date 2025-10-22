#ifndef _DATA_STRUCTURE_H_
#define _DATA_STRUCTURE_H_

// 作用：通用数据结构
// 作者：chong.xiao
// 时间：2024.11.5

#define Sy_CV_CN_MAX     512
#define Sy_CV_CN_SHIFT   3
#define Sy_CV_DEPTH_MAX  (1 << Sy_CV_CN_SHIFT)

#define Sy_CV_8U   0
#define Sy_CV_8S   1
#define Sy_CV_16U  2
#define Sy_CV_16S  3
#define Sy_CV_32S  4
#define Sy_CV_32F  5
#define Sy_CV_64F  6
#define Sy_CV_16F  7

#define Sy_CV_MAT_DEPTH_MASK       (Sy_CV_DEPTH_MAX - 1)
#define Sy_CV_MAT_DEPTH(flags)     ((flags) & Sy_CV_MAT_DEPTH_MASK)

#define Sy_CV_MAKETYPE(depth,cn) (Sy_CV_MAT_DEPTH(depth) + (((cn)-1) << Sy_CV_CN_SHIFT))
#define Sy_CV_MAKE_TYPE Sy_CV_MAKETYPE

#define Sy_CV_8UC1 Sy_CV_MAKETYPE(Sy_CV_8U,1)
#define Sy_CV_8UC2 Sy_CV_MAKETYPE(Sy_CV_8U,2)
#define Sy_CV_8UC3 Sy_CV_MAKETYPE(Sy_CV_8U,3)
#define Sy_CV_8UC4 Sy_CV_MAKETYPE(Sy_CV_8U,4)
#define Sy_CV_8UC(n) Sy_CV_MAKETYPE(Sy_CV_8U,(n))

#define Sy_CV_8SC1 Sy_CV_MAKETYPE(Sy_CV_8S,1)
#define Sy_CV_8SC2 Sy_CV_MAKETYPE(Sy_CV_8S,2)
#define Sy_CV_8SC3 Sy_CV_MAKETYPE(Sy_CV_8S,3)
#define Sy_CV_8SC4 Sy_CV_MAKETYPE(Sy_CV_8S,4)
#define Sy_CV_8SC(n) Sy_CV_MAKETYPE(Sy_CV_8S,(n))

#define Sy_CV_16UC1 Sy_CV_MAKETYPE(Sy_CV_16U,1)
#define Sy_CV_16UC2 Sy_CV_MAKETYPE(Sy_CV_16U,2)
#define Sy_CV_16UC3 Sy_CV_MAKETYPE(Sy_CV_16U,3)
#define Sy_CV_16UC4 Sy_CV_MAKETYPE(Sy_CV_16U,4)
#define Sy_CV_16UC(n) Sy_CV_MAKETYPE(Sy_CV_16U,(n))

#define Sy_CV_16SC1 Sy_CV_MAKETYPE(Sy_CV_16S,1)
#define Sy_CV_16SC2 Sy_CV_MAKETYPE(Sy_CV_16S,2)
#define Sy_CV_16SC3 Sy_CV_MAKETYPE(Sy_CV_16S,3)
#define Sy_CV_16SC4 Sy_CV_MAKETYPE(Sy_CV_16S,4)
#define Sy_CV_16SC(n) Sy_CV_MAKETYPE(Sy_CV_16S,(n))

#define Sy_CV_32SC1 Sy_CV_MAKETYPE(Sy_CV_32S,1)
#define Sy_CV_32SC2 Sy_CV_MAKETYPE(Sy_CV_32S,2)
#define Sy_CV_32SC3 Sy_CV_MAKETYPE(Sy_CV_32S,3)
#define Sy_CV_32SC4 Sy_CV_MAKETYPE(Sy_CV_32S,4)
#define Sy_CV_32SC(n) Sy_CV_MAKETYPE(Sy_CV_32S,(n))

#define Sy_CV_32FC1 Sy_CV_MAKETYPE(Sy_CV_32F,1)
#define Sy_CV_32FC2 Sy_CV_MAKETYPE(Sy_CV_32F,2)
#define Sy_CV_32FC3 Sy_CV_MAKETYPE(Sy_CV_32F,3)
#define Sy_CV_32FC4 Sy_CV_MAKETYPE(Sy_CV_32F,4)
#define Sy_CV_32FC(n) Sy_CV_MAKETYPE(Sy_CV_32F,(n))

#define Sy_CV_64FC1 Sy_CV_MAKETYPE(Sy_CV_64F,1)
#define Sy_CV_64FC2 Sy_CV_MAKETYPE(Sy_CV_64F,2)
#define Sy_CV_64FC3 Sy_CV_MAKETYPE(Sy_CV_64F,3)
#define Sy_CV_64FC4 Sy_CV_MAKETYPE(Sy_CV_64F,4)
#define Sy_CV_64FC(n) Sy_CV_MAKETYPE(Sy_CV_64F,(n))

#define Sy_CV_16FC1 Sy_CV_MAKETYPE(Sy_CV_16F,1)
#define Sy_CV_16FC2 Sy_CV_MAKETYPE(Sy_CV_16F,2)
#define Sy_CV_16FC3 Sy_CV_MAKETYPE(Sy_CV_16F,3)
#define Sy_CV_16FC4 Sy_CV_MAKETYPE(Sy_CV_16F,4)
#define Sy_CV_16FC(n) Sy_CV_MAKETYPE(Sy_CV_16F,(n))

//颜色转换标志
enum SyColorConversionCodes {

	Sy_COLOR_NOCHANGE = -1, //图像不改变

	Sy_COLOR_BGR2RGB = 4,

	Sy_COLOR_RGB2BGR = Sy_COLOR_BGR2RGB,

	Sy_COLOR_BGR2GRAY = 6,

	Sy_COLOR_RGB2GRAY = 7,

	Sy_COLOR_GRAY2BGR = 8,

	Sy_COLOR_GRAY2RGB = Sy_COLOR_GRAY2BGR,

	Sy_COLOR_BGR2HSV = 40,

	Sy_COLOR_RGB2HSV = 41,

	Sy_COLOR_BGR2Lab = 44,

	Sy_COLOR_RGB2Lab = 45,

	Sy_COLOR_BGR2HLS = 52,

	Sy_COLOR_RGB2HLS = 53,

	Sy_COLOR_HSV2BGR = 54,

	Sy_COLOR_HSV2RGB = 55,

	Sy_COLOR_Lab2BGR = 56,

	Sy_COLOR_Lab2RGB = 57,

	Sy_COLOR_Luv2BGR = 58,

	Sy_COLOR_Luv2RGB = 59,

	Sy_COLOR_HLS2BGR = 60,

	Sy_COLOR_HLS2RGB = 61,
};
// 三维点
typedef struct {
	float x; // 点的x坐标
	float y; // 点的y坐标
	float z; // 点的z坐标
} SyPoint3D;

// 二维点
typedef struct {
	float x; // 点的x坐标
	float y; // 点的y坐标
} SyPoint2D;

// 线段
typedef struct {
	SyPoint2D start; // 线段的起点
	SyPoint2D end;   // 线段的终点
} LineSegment;

// 圆
typedef struct {
	SyPoint2D center; // 圆心点
	float radius; // 圆的半径
} SyCircle;

// 矩形
typedef struct {
	SyPoint2D topLeft; // 矩形的左上角点
	SyPoint2D bottomRight; // 矩形的右下角点
} SyRectangle;

// 图像
struct SyImage {
	int	width; // 图像的宽度
	int	height; // 图像的高度
	int type;//图像数据类型
	unsigned char* data; // 指向图像数据的指针（例如，像素数据）
	size_t step = 0;//单行图像所占的字节数
} ;

#endif // !
