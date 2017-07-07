#pragma once

#include <ProSection.h>
#include <ProStdSection.h>
#include <vector>
#include "CreoMFCDialog.h"

using namespace std;
// CCreateMidiCurveDlg 对话框
#define EPSM6                 1.0e-6
#define PI              3.1415926

typedef struct section_info
{
	ProSection3d section;  /* initialized section handle */
	ProSelection sketch_pln;
	ProUvParam sketch_uv;
	ProSecViewDirType view_dir;
	ProSelection orient_pln;
	ProSecOrientDirType orient_dir;
} ProTestSectionInfo;

static double identity_matrix[4][4] = { { 1.0, 0.0, 0.0, 0.0 },
{ 0.0, 1.0, 0.0, 0.0 },
{ 0.0, 0.0, 1.0, 0.0 },
{ 0.0, 0.0, 0.0, 1.0 } };

int ProUtilLinesPerpend(double line1[2][3], double line2[2][3]);
double *ProUtilVectorCopy(double from[3], double to[3]);
double *ProUtilVectorDiff(double a[3], double b[3], double c[3]);
double *ProUtilVectorNormalize(double input[3], double output[3]);
double ProUtilVectorDot(double a[3], double b[3]);
double ProUtilVectorLength(double v[3]);
double *ProUtilVectorScale(double scalar, double vector[3], double result[3]);
void ProUtilPointTrans(double m[4][4], double p[3], double output[3]);
double *ProUtilLineLineX(double l1[2][3], double l2[2][3], double int_pnt[3]);
int ProUtilLinesParallel(double line1[2][3], double line2[2][3]);
double *ProUtilVectorCross(double v1[3], double v2[3], double c[3]);
double *ProUtilPlaneLineX(double plane_point[3], /* IN - point on the plane */ double plane_normal[3], /* IN - normal to the plane */ double line_point[3], /* IN - point on the line */ double line_dir[3], /* IN - direction of the line */ double x_point[3]) /* OUT - intersection point */;
double *ProUtilVectorsLincom(double scalar_a, double vector_a[3], double scalar_b, double vector_b[3], double result[3]);
int ProUtilMatrixInvert(double m[4][4], double output[4][4]);
void ProUtilMatrixCopy(double input[4][4], double output[4][4]);
void DrawDLine(ProSection aSec, ProTestSectionInfo *p_sketch_refs, ProMatrix transform, double xOffset, double yOffset, double aStartPt, double aEndPt, double fHeight, double aAngleA, double aAngleB, double(&aRealEndPt)[2], vector<int> & ent_ids);
void DrawTriangle(ProSection aSec, ProTestSectionInfo *p_sketch_refs, ProMatrix transform, double xOffset, double yOffset, double aStartPt, double aPt, double aEndPt, double fHeight, vector<int> & ent_ids);
void DrawELine(ProSection aSec, ProTestSectionInfo *p_sketch_refs, ProMatrix transform, double xOffset, double yOffset, double aStartPt, double aEndPt, vector<int> & ent_ids);
void ComputeTriangleStartAndEndPointByMidPt(double aPt, double fHeight, double aAngleA, double aAngleB, double(&aRealPt)[2]);
void ComputeTriangleStartAndEndPointByStartPt(double aPt, double fHeight, double aAngleA, double aAngleB, double(&aRealPt)[2]);

ProError ProUtilSectionClean(ProSection section);
class CCreateMidiCurveDlg : public CCreoMFCDialog
{
	DECLARE_DYNAMIC(CCreateMidiCurveDlg)

public:
	CCreateMidiCurveDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCreateMidiCurveDlg();

	static CCreateMidiCurveDlg *GetInstance(CWnd* pParent);
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum {IDD = IDD_CreateMidiCurveDlg};
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	static int m_iDlgNum;
	static CCreateMidiCurveDlg * m_pDlgInstance;

	vector<vector<double>> m_pMidiDes;
	virtual void PostNcDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	void GetMidiInfoFromFile(CString strFileName);
	ProError ProTestSectionBuild(ProSection section, ProType sec_type, ProTestSectionInfo *p_sketch_refs, void *dummy);
	ProError ProTestBoxSectBuild(ProSection section, ProType sec_type, ProTestSectionInfo *p_sketch_refs);
	afx_msg void OnBnClickedButton1();
	CString m_strPath;
	CString m_strOutputFileName;
	double m_fTotalLength;
	double m_fHeightVelogicRation;
	double m_fAngleA;
	double m_fAngleB;
};
