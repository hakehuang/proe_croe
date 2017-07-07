 /* Copyright (C) 2017 Shanghai Yuanxue Machinery Technology Co., Ltd.
 *
 * All rights reserved
 */

// CreateMidiCurveDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Midi.h"
#include "CreateMidiCurveDlg.h"

#include <math.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>

#include "global.h"

#include <ProCurve.h>
#include <ProUtil.h>
#include <ProSecdim.h>
#include <ProSurface.h>
#include <ProWindows.h>
#include <ProArray.h>
#include <ProMessage.h>
// CCreateMidiCurveDlg 对话框
int CCreateMidiCurveDlg::m_iDlgNum = 0;
CCreateMidiCurveDlg* CCreateMidiCurveDlg::m_pDlgInstance = NULL;
IMPLEMENT_DYNAMIC(CCreateMidiCurveDlg, CCreoMFCDialog)


CCreateMidiCurveDlg::CCreateMidiCurveDlg(CWnd* pParent /*=NULL*/)
	: CCreoMFCDialog(IDD_CreateMidiCurveDlg, pParent)
	, m_strPath(_T(""))
	, m_strOutputFileName(_T(""))
	, m_fTotalLength(0)
	, m_fHeightVelogicRation(0)
	, m_fAngleA(0)
	, m_fAngleB(0)
{

}

CCreateMidiCurveDlg::~CCreateMidiCurveDlg()
{
	for (int i=0;i<m_pMidiDes.size();i++)
	{
		vector<double> v = m_pMidiDes[i];
		v.clear();
	}
	m_pMidiDes.clear();
}

void CCreateMidiCurveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strPath);
	DDX_Text(pDX, IDC_EDIT4, m_strOutputFileName);
	DDX_Text(pDX, IDC_EDIT5, m_fTotalLength);
	DDX_Text(pDX, IDC_EDIT6, m_fHeightVelogicRation);
	DDX_Text(pDX, IDC_EDIT2, m_fAngleA);
	DDV_MinMaxDouble(pDX, m_fAngleA, 0, 90);
	DDX_Text(pDX, IDC_EDIT3, m_fAngleB);
	DDV_MinMaxDouble(pDX, m_fAngleB, 0, 90);
}


BEGIN_MESSAGE_MAP(CCreateMidiCurveDlg, CCreoMFCDialog)
	ON_BN_CLICKED(IDOK, &CCreateMidiCurveDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &CCreateMidiCurveDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CCreateMidiCurveDlg 消息处理程序
CCreateMidiCurveDlg *CCreateMidiCurveDlg::GetInstance(CWnd* pParent)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (m_pDlgInstance == NULL)
	{
		m_pDlgInstance = new CCreateMidiCurveDlg(pParent);
		m_pDlgInstance->Create(IDD_CreateMidiCurveDlg);
		m_iDlgNum++;
	}
	return m_pDlgInstance;
}

void CCreateMidiCurveDlg::PostNcDestroy()
{
	// TODO: 在此添加专用代码和/或调用基类
	m_iDlgNum--;
	CCreoMFCDialog::PostNcDestroy();
	m_pDlgInstance = NULL;
}


BOOL CCreateMidiCurveDlg::OnInitDialog()
{
	CCreoMFCDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	UpdateData(TRUE);
	m_strOutputFileName = L"Midi";
	m_fTotalLength = 300;
	m_fHeightVelogicRation = 0.2;
	m_fAngleA = 45;
	m_fAngleB = 45;
	UpdateData(FALSE);
	

	
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CCreateMidiCurveDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	ProSection section;
	ProError status;
	if (m_strPath.IsEmpty())
	{
		MessageBox(L"Please select Midi file!", L"Error", MB_ICONSTOP | MB_OK);
		return;
	}
	else
	{
		GetMidiInfoFromFile(m_strPath);
	}
	UpdateData(TRUE);
	if ((fcmp(m_fTotalLength,0)==0)||(fcmp(m_fHeightVelogicRation,0)==0)||(fcmp(m_fAngleA,0)==0)||(fcmp(m_fAngleB,0)==0))
	{
		MessageBox(L"config is wrong!", L"Error", MB_ICONSTOP | MB_OK);
		return;
	}
	else
	{
		if (fcmp(m_fAngleA + m_fAngleB, 90) > 180)
		{
			MessageBox(L"α plus β must less than 180 !", L"Error", MB_ICONSTOP | MB_OK);
			return;
		}
		else if ((fcmp(m_fAngleA, 90) >= 0) || (fcmp(m_fAngleB, 90) >= 0))
		{
			MessageBox(L"α or β must less than 90 !", L"Error", MB_ICONSTOP | MB_OK);
			return;
		}
	}
	UpdateData(FALSE);

	ProMdl aMdl;
	status=ProMdlRetrieve(m_strOutputFileName.AllocSysString(), PRO_MDL_2DSECTION, &aMdl);
	if (status==PRO_TK_NO_ERROR)
	{
		int ret=MessageBox(L"There is a file have same name,recover it?",L"Warning",MB_ICONQUESTION | MB_OKCANCEL);
		if (ret == IDOK)
		{
			ProUtilSectionClean((ProSection)aMdl);
			status = ProTestSectionBuild((ProSection)aMdl, PRO_2DSECTION, NULL, NULL);

			status = ProMdlSave(aMdl);

			ret = MessageBox(L"Midi curve file has been created,open it?", L"", MB_OKCANCEL | MB_ICONQUESTION);
			if (ret == IDOK)
			{
					int winId;
					ProMdlDisplay(aMdl);
					status = ProWindowCurrentGet(&winId);
					ProWindowActivate(winId);
			}
		}
		else
		{
			return;
		}
	}
	else
	{
		status = ProSection2DAlloc(&section);

		status = ProTestSectionBuild(section, PRO_2DSECTION, NULL, NULL);

		status = ProSectionNameSet(section, m_strOutputFileName.AllocSysString());

		status = ProMdlSave((ProMdl)section);
		status = ProSectionFree(&section);


		int ret = MessageBox(L"Midi curve file has been created,open it?", L"", MB_OKCANCEL | MB_ICONQUESTION);
		if (ret == IDOK)
		{
			status = ProMdlRetrieve(m_strOutputFileName.AllocSysString(), PRO_MDL_2DSECTION, &aMdl);
			if (status == PRO_TK_NO_ERROR)
			{
				int winId;
				ProMdlDisplay(aMdl);
				status = ProWindowCurrentGet(&winId);
				ProWindowActivate(winId);
			}
		}
	}
}

void CCreateMidiCurveDlg::GetMidiInfoFromFile(CString strFileName)
{
	ifstream file(strFileName);
	double tmp_d;
	char tmp_c;
	string tmp;
	stringstream ss;
	
	for (int i = 0;i < m_pMidiDes.size();i++)
	{
		vector<double> v = m_pMidiDes[i];
		v.clear();
	}
	m_pMidiDes.clear();

	while (file >> tmp ) 
	{
		ss << tmp;
		vector<double> v;
		for (int i = 0;i < 3;++i)
		{
			ss >> tmp_c >> tmp_d;
			v.push_back(tmp_d);
		}
		ss.get();
		m_pMidiDes.push_back(v);
	}
}

ProError CCreateMidiCurveDlg::ProTestSectionBuild(
	ProSection section, /* In: Pre-Allocated Workspace section handle */
	ProType sec_type, /* 2D or 3D section */
	ProTestSectionInfo *p_sketch_refs, /* In: sketch pln info for 3D sec */
	void *dummy)    /* Out: Type of section to create */
{

	return ProTestBoxSectBuild(section, sec_type, p_sketch_refs);
}

/*====================================================================*\
FUNCTION :  ProTestBoxSectBuild()
PURPOSE  :  Create a box section
\*====================================================================*/
ProError CCreateMidiCurveDlg::ProTestBoxSectBuild(
	ProSection section,
	ProType sec_type,
	ProTestSectionInfo *p_sketch_refs)
{
	ProError status;
	Pro2dLinedef line;
	vector<int > ent_ids;
	double width, height, offset;
	Pro2dPnt dim_pnt;
	int entdim_ids[2];
	int dim_ids[5];
	double edge1[2][3], edge2[2][3], offsets[2];
	double sk_transf[4][4];
	double transform[4][4];
	double angle = 0.0;
	ProSectionPointType pnt_types[2];
	ProLine w_message;
	ProWSecerror sec_errors;

	width = 12.0;
	height = 10.0;

	offset = width / 2.0;
	offsets[0] = offsets[1] = offset;

	double eps= EPSM6;
	status = ProSectionEpsilonSet(section, eps);
	
	double TrackTime = 600;
	double TrackLength = m_fTotalLength;//300
	double fTimeScale = TrackLength/TrackTime;
	
	double MidiVelocy = 9;
	double MidiHeight = 3;
	double fHeightScale = m_fHeightVelogicRation;//MidiHeight/ MidiVelocy;

	double tmpPoint[2];
	double startPoint =0;
	double fAngleA = m_fAngleA*PI/180; //PI / 4;
	double fAngleB = m_fAngleB*PI / 180;//PI / 4;

	for (int i=0;i<m_pMidiDes.size();i++)
	{
		DrawDLine(section, p_sketch_refs, transform, offset, offset, startPoint, m_pMidiDes[i][0]*fTimeScale, m_pMidiDes[i][2]*fHeightScale, fAngleA, fAngleB,tmpPoint, ent_ids);
		DrawTriangle(section, p_sketch_refs, transform, offset, offset, tmpPoint[0], m_pMidiDes[i][0]*fTimeScale, tmpPoint[1], m_pMidiDes[i][2]*fHeightScale, ent_ids);
		startPoint = tmpPoint[1];
	}

	if (fcmp(TrackLength,tmpPoint[1]) > 0)
	{
		DrawELine(section, p_sketch_refs, transform, offset, offset, tmpPoint[1], TrackLength, ent_ids);
	}

// 	if (p_sketch_refs != NULL)
// 	{
// 		status = ProUtilTwoOrthogEdgesOnPlaneGet(section, &ent_ids[5],
// 			edge1, edge2);
// 		if (status != PRO_TK_NO_ERROR)
// 			return(PRO_TK_GENERAL_ERROR);
// 
// 		ProUtilSketchCsysGet(p_sketch_refs, sk_transf);
// 
// 		ProUtilSectCsysTransformGet(p_sketch_refs, sk_transf, edge1, edge2, transform, &angle);
// 		if ((fabs(transform[0][0])>1e-3) ^(!(fabs(edge1[0][0] - edge1[1][0])>1e-3)))
// 		{
// 			offset = offsets[0];offsets[0] = offsets[1];offsets[1] = offset;
// 		}
// 	}

// 	line.type = PRO_2D_LINE;
// 	line.end1[0] = offsets[0];
// 	line.end1[1] = offsets[1] + height;
// 	line.end2[0] = offsets[0] + width;
// 	line.end2[1] = offsets[1] + height;
// 	if (p_sketch_refs != NULL)
// 	{
// 		ProUtil2DPointTrans(transform, line.end1, line.end1);
// 		ProUtil2DPointTrans(transform, line.end2, line.end2);
// 	}
// 	status = ProSectionEntityAdd(section, (Pro2dEntdef *)&line, &ent_ids[2]);
// 
// 	line.type = PRO_2D_LINE;
// 	line.end1[0] = offsets[0] + width;
// 	line.end1[1] = offsets[1] + height;
// 	line.end2[0] = offsets[0] + width;
// 	line.end2[1] = offsets[1];
// 	if (p_sketch_refs != NULL)
// 	{
// 		ProUtil2DPointTrans(transform, line.end1, line.end1);
// 		ProUtil2DPointTrans(transform, line.end2, line.end2);
// 	}
// 	status = ProSectionEntityAdd(section, (Pro2dEntdef *)&line, &ent_ids[3]);
// 
// 	line.type = PRO_2D_LINE;
// 	line.end1[0] = offsets[0] + width;
// 	line.end1[1] = offsets[1];
// 	line.end2[0] = offsets[0];
// 	line.end2[1] = offsets[1];
// 	if (p_sketch_refs != NULL)
// 	{
// 		ProUtil2DPointTrans(transform, line.end1, line.end1);
// 		ProUtil2DPointTrans(transform, line.end2, line.end2);
// 	}
// 	status = ProSectionEntityAdd(section, (Pro2dEntdef *)&line,
// 		&ent_ids[4]);

// 	/*--------------------------------------------------------------------*\
// 	Dimension the box
// 	\*--------------------------------------------------------------------*/
// 	dim_pnt[0] = offsets[0] - 1;
// 	dim_pnt[1] = offsets[1] + height / 2;
// 	if (p_sketch_refs != NULL)
// 	{
// 		ProUtil2DPointTrans(transform, dim_pnt, dim_pnt);
// 	}
// 	entdim_ids[0] = ent_ids[2];
// 	entdim_ids[1] = ent_ids[4];
// 	pnt_types[0] = PRO_ENT_WHOLE;
// 	pnt_types[1] = PRO_ENT_WHOLE;
// 	status = ProSecdimCreate(section, entdim_ids, pnt_types, 2,	PRO_TK_DIM_LINE_LINE, dim_pnt, &dim_ids[0]);
// 
// 	dim_pnt[0] = offsets[0] - width / 2;
// 	dim_pnt[1] = offsets[1] + 1.0;
// 	entdim_ids[0] = ent_ids[2];
// 	pnt_types[0] = PRO_ENT_WHOLE;
// 	if (p_sketch_refs != NULL)
// 	{
// 		ProUtil2DPointTrans(transform, dim_pnt, dim_pnt);
// 	}
// 	status = ProSecdimCreate(section, entdim_ids, pnt_types, 1,PRO_TK_DIM_LINE, dim_pnt, &dim_ids[1]);
	/*--------------------------------------------------------------------*\
	Autodim section
	\*--------------------------------------------------------------------*/
// 	ProStringToWstring(w_message, (char *)"ProSectionAutodim() errors.");
// 	status = ProSecerrorAdd(&sec_errors, 0, w_message, 0, -1, 0);
	status = ProSecerrorAlloc(&sec_errors);
	status = ProSectionAutodim(section, &sec_errors);
	status = ProSecerrorFree(&sec_errors);
	/*--------------------------------------------------------------------*\
	Solve and regenerate the section
	\*--------------------------------------------------------------------*/
	status = ProSectionRegenerate(section, &sec_errors);
	if (status != PRO_TK_NO_ERROR)
	{
		return(PRO_TK_GENERAL_ERROR);
	}

	return(PRO_TK_NO_ERROR);
}

/*====================================================================*\
FUNCTION :	ProUtilTwoOrthogEdgesOnPlaneGet
PURPOSE  :	Gets the selection of two orthogonal edges on sketch
plane from the user
\*====================================================================*/
ProError ProUtilTwoOrthogEdgesOnPlaneGet(
	ProSection section,
	int ent_ids[2],
	double edge1[2][3],
	double edge2[2][3])
{

	ProError status;
	double edges[2][2][3];
	ProSelection *p_sels;
	int n_edges, i;
	int error = 0, cont = 1;
	Pro2dLinedef *linedef;

	while (cont)
	{
		ProMessageDisplay(USER_MESSAGE_FILE, "USER INFO:%0s", (char*)"Select two orthogonal edges.");
		status = ProSelect((char*)"edge", 2, NULL, NULL, NULL, NULL, &p_sels, &n_edges);
		

		if (n_edges < 2) return(PRO_TK_GENERAL_ERROR);

		for (i = 0; i<2; i++)
		{
			status = ProSectionEntityFromProjection(section, p_sels[i],
				&ent_ids[i]);
			

			if (status == PRO_TK_NO_ERROR)
			{
				status = ProSectionEntityGet(section, ent_ids[i],
					(Pro2dEntdef **)&linedef);
				
			}
			else
			{
				ProMessageDisplay(USER_MESSAGE_FILE, "USER INFO:%0s", (char*)"TEST One of the edges is perpendicular to sketch plane.");
				error = 1;
				break;
			}

			edges[i][0][0] = linedef->end1[0];
			edges[i][0][1] = linedef->end1[1];
			edges[i][0][2] = 0.0;
			edges[i][1][0] = linedef->end2[0];
			edges[i][1][1] = linedef->end2[1];
			edges[i][1][2] = 0.0;
		}
		if (error)
		{
			error = 0;
			continue;
		}

		if (!ProUtilLinesPerpend(edges[0], edges[1]))
		{
			ProMessageDisplay(USER_MESSAGE_FILE, "USER INFO:%0s", (char*)"TEST Edges are not orthogonal.");
			continue;
		}
		else
		{
			ProUtilVectorCopy(edges[0][0], edge1[0]);
			ProUtilVectorCopy(edges[0][1], edge1[1]);
			ProUtilVectorCopy(edges[1][0], edge2[0]);
			ProUtilVectorCopy(edges[1][1], edge2[1]);
			break;
		}
	}

	return(PRO_TK_NO_ERROR);
}

/*====================================================================*\
FUNCTION :	ProUtilLinesPerpend
PURPOSE  :	Check if two lines are perpendicular (returns 1)
\*====================================================================*/
int ProUtilLinesPerpend(double line1[2][3], double line2[2][3])
{
	double vec1[3], vec2[3];

	ProUtilVectorDiff(line1[0], line1[1], vec1);
	ProUtilVectorDiff(line2[0], line2[1], vec2);

	ProUtilVectorNormalize(vec1, vec1);
	ProUtilVectorNormalize(vec2, vec2);

	if (fabs(ProUtilVectorDot(vec1, vec2)) <= EPSM6)
		return(1);

	return(0);
}

/*====================================================================*\
FUNCTION :	ProUtilVectorCopy()
PURPOSE  :	Copy a vector to another
\*====================================================================*/
double *ProUtilVectorCopy(
	double from[3],
	double to[3])
{
	if (from == NULL)
		to[0] = to[1] = to[2] = 0.0;
	else
	{
		to[0] = from[0];
		to[1] = from[1];
		to[2] = from[2];
	}
	return(to);
}

/*====================================================================*\
FUNCTION :	ProUtilVectorDiff()
PURPOSE  :	Difference of two vectors
\*====================================================================*/
double *ProUtilVectorDiff(
	double a[3],
	double b[3],
	double c[3])
{
	c[0] = a[0] - b[0];
	c[1] = a[1] - b[1];
	c[2] = a[2] - b[2];
	return(c);
}
/*====================================================================*\
FUNCTION :	ProUtilVectorNormalize()
PURPOSE  :	Normalize a vector
\*====================================================================*/
double *ProUtilVectorNormalize(
	double input[3],
	double output[3])
{
	double len;
	len = ProUtilVectorLength(input);
	return(ProUtilVectorScale(1.0 / len, input, output));
}
/*====================================================================*\
FUNCTION :	ProUtilVectorDot()
PURPOSE  :	Dot product of two vectors
\*====================================================================*/
double ProUtilVectorDot(
	double a[3],
	double b[3])
{
	double dot;
	dot = a[2] * b[2] + a[1] * b[1] + a[0] * b[0];
	return(dot);
}
/*====================================================================*\
FUNCTION :	ProUtilVectorLength()
PURPOSE  :	Length of a vector
\*====================================================================*/
double ProUtilVectorLength(
	double v[3])
{
	return(sqrt(v[0] * v[0] +
		v[1] * v[1] +
		v[2] * v[2]));
}
/*====================================================================*\
FUNCTION :	ProUtilVectorScale()
PURPOSE  :	Multiply a vector by a scalar
\*====================================================================*/
double *ProUtilVectorScale(
	double scalar,
	double vector[3],
	double result[3])
{
	result[0] = scalar * vector[0];
	result[1] = scalar * vector[1];
	result[2] = scalar * vector[2];
	return(result);
}

void ProUtil2DPointTrans(
	double m[4][4],
	double p2D[2],
	double out2D[2])
{
	double p3D[3];
	double out3D[3];

	p3D[0] = p2D[0];
	p3D[1] = p2D[1];
	p3D[2] = 0.0;

	ProUtilPointTrans(m, p3D, out3D);

	out2D[0] = out3D[0];
	out2D[1] = out3D[1];
}

/*====================================================================*\
FUNCTION :	ProUtilPointTrans()
PURPOSE  :	Transform a 3d point by a matrix
\*====================================================================*/
void ProUtilPointTrans(
	double m[4][4],
	double p[3],
	double output[3])
{
	double buff[4];
	ProError status;

	if (m == NULL)
	{
		ProUtilVectorCopy(p, output);
		return;
	}

	status = ProPntTrfEval(p, m, buff);
	ProUtilVectorCopy(buff, output);

}

/*====================================================================*\
FUNCTION :	ProUtilSectCsysTransformGet
PURPOSE  :	Given a plane selection and two orthogonal lines on
the plane defining a csys, output transform from this
csys to plane csys (selection point is used to determine
x and y vectors such that the point is in the first
quadrant) (currently works for part mode)
\*====================================================================*/
ProError ProUtilSectCsysTransformGet(
	ProTestSectionInfo *p_sketch_refs,
	double sk_transf[4][4],
	double l1[2][3],
	double l2[2][3],
	double transform[4][4],
	double *angle)
{

	ProMatrix inv_sk_transf;
	Pro3dPnt origin, sel_pnt, proj_sel_pnt;
	ProVector n, v, n1, plane_x;
	ProVector x, y, z_vec = { 0,0,1 };
	double dot;
	ProSurface p_plane;
	ProModelitem mdlitem;
	ProError status;

	ProUtilLineLineX(l1, l2, origin);

	status = ProSelectionModelitemGet(p_sketch_refs->sketch_pln, &mdlitem);

	status = ProSurfaceInit(mdlitem.owner, mdlitem.id, &p_plane);


	status = ProSurfaceXyzdataEval(p_plane, p_sketch_refs->sketch_uv,
		sel_pnt, NULL, NULL, NULL);


	ProUtilVectorDiff(l1[0], l1[1], x);
	ProUtilVectorNormalize(x, x);
	ProUtilVectorDiff(l2[0], l2[1], y);
	ProUtilVectorNormalize(y, y);

	ProUtilMatrixInvert(sk_transf, inv_sk_transf);
	ProUtilPointTrans(inv_sk_transf, sel_pnt, proj_sel_pnt);
	proj_sel_pnt[2] = 0.0;

	ProUtilVectorDiff(proj_sel_pnt, origin, v);
	v[2] = 0.0;
	ProUtilVectorNormalize(v, v);

	/* make sure sel pnt is in first quadrant */

	dot = ProUtilVectorDot(v, x);
	if (dot < 0.0)
		ProUtilVectorScale(-1.0, x, x);
	dot = ProUtilVectorDot(v, y);
	if (dot < 0.0)
		ProUtilVectorScale(-1.0, y, y);

	/* make sure x and y are correct relative to plane normal */
	ProUtilVectorCross(x, y, n);
	if (n[2] < 0.0)
	{
		ProUtilVectorCopy(x, n);
		ProUtilVectorCopy(y, x);
		ProUtilVectorCopy(n, y);
	}

	status = ProMatrixInit(x, y, z_vec, origin, transform);

	/* calculate rotation angle */
	plane_x[0] = 1.0;
	plane_x[1] = plane_x[2] = 0.0;
	ProUtilVectorCross(x, y, n);
	ProUtilVectorCross(plane_x, x, n1);
	*angle = fabs(acos(ProUtilVectorDot(plane_x, x)));
	if (ProUtilVectorDot(n, n1) < 0.0)
		*angle *= -1.0;

	return(PRO_TK_NO_ERROR);
}

/*====================================================================*\
FUNCTION :	ProUtilLineLineX
PURPOSE  :	Get the intersection point of two lines
\*====================================================================*/
double *ProUtilLineLineX(
	double l1[2][3],
	double l2[2][3],
	double int_pnt[3])
{
	double v1[3], v2[3], v3[3], v4[4];
	double n[3], n1[3], n2[3];

	/* if lines parallel return NULL */
	if (ProUtilLinesParallel(l1, l2))
		return(NULL);

	ProUtilVectorDiff(l1[0], l1[1], v1);
	ProUtilVectorDiff(l2[0], l2[1], v2);

	if ((fabs(l1[0][0] - l2[0][0]) <= EPSM6) &&
		(fabs(l1[0][1] - l2[0][1]) <= EPSM6) &&
		(fabs(l1[0][2] - l2[0][2]) <= EPSM6))
	{
		memcpy(int_pnt, l1[0], 3 * sizeof(double));
		return(int_pnt);
	}

	if ((fabs(l1[1][0] - l2[1][0]) <= EPSM6) &&
		(fabs(l1[1][1] - l2[1][1]) <= EPSM6) &&
		(fabs(l1[1][2] - l2[1][2]) <= EPSM6))
	{
		memcpy(int_pnt, l1[1], 3 * sizeof(double));
		return(int_pnt);
	}

	ProUtilVectorDiff(l1[0], l2[0], v3);
	ProUtilVectorDiff(l1[1], l2[1], v4);
	ProUtilVectorCross(v1, v2, n1);
	ProUtilVectorCross(v3, v4, n2);
	ProUtilVectorNormalize(n1, n1);
	ProUtilVectorNormalize(n2, n2);

	/* if lines not on same plane return NULL */
	if ((1.0 - fabs(ProUtilVectorDot(n1, n2))) >= EPSM6)
		return(NULL);

	ProUtilVectorNormalize(v1, v1);
	ProUtilVectorNormalize(v2, v2);
	ProUtilVectorCross(n1, v1, n);
	ProUtilPlaneLineX(l1[0], n, l2[0], v2, int_pnt);

	return(int_pnt);

}

/*====================================================================*\
FUNCTION :	ProUtilLinesParallel
PURPOSE  :	Check if two lines are parallel (returns 1) to each other
\*====================================================================*/
int ProUtilLinesParallel(double line1[2][3], double line2[2][3])
{
	double vec1[3], vec2[3];

	ProUtilVectorDiff(line1[0], line1[1], vec1);
	ProUtilVectorDiff(line2[0], line2[1], vec2);

	ProUtilVectorNormalize(vec1, vec1);
	ProUtilVectorNormalize(vec2, vec2);

	if ((1.0 - fabs(ProUtilVectorDot(vec1, vec2))) <= EPSM6)
		return(1);

	return(0);
}

/*====================================================================*\
FUNCTION :	ProUtilVectorCross()
PURPOSE  :	Cross product of two vectors
\*====================================================================*/
double *ProUtilVectorCross(
	double v1[3],
	double v2[3],
	double c[3])
{
	c[0] = v1[1] * v2[2] - v1[2] * v2[1];
	c[1] = v1[2] * v2[0] - v1[0] * v2[2];
	c[2] = v1[0] * v2[1] - v1[1] * v2[0];

	return(c);
}

/*====================================================================*\
FUNCTION :	ProUtilPlaneLineInter()
PURPOSE  :	Intersection between a plane and a line
\*====================================================================*/
double *ProUtilPlaneLineX(
	double plane_point[3],	/* IN - point on the plane	*/
	double plane_normal[3],	/* IN - normal to the plane	*/
	double line_point[3],	/* IN - point on the line	*/
	double line_dir[3],		/* IN - direction of the line	*/
	double x_point[3])		/* OUT - intersection point	*/
{
	double proj, vector[3], dist;

	proj = ProUtilVectorDot(plane_normal, line_dir);

	/*--------------------------------------------------------------------*\
	If the line is parallel to the plane, return NULL
	\*--------------------------------------------------------------------*/
	if (fabs(proj) < EPSM6)
		return(NULL);

	ProUtilVectorDiff(plane_point, line_point, vector);
	dist = ProUtilVectorDot(vector, plane_normal) / proj;
	ProUtilVectorsLincom(1.0, line_point, dist, line_dir, x_point);
	return(x_point);
}

/*====================================================================*\
FUNCTION :	ProUtilVectorsLincom()
PURPOSE  :	Linear combination of two vectors
\*====================================================================*/
double *ProUtilVectorsLincom(
	double scalar_a,
	double vector_a[3],
	double scalar_b,
	double vector_b[3],
	double result[3])
{
	result[0] = scalar_a * vector_a[0] + scalar_b * vector_b[0];
	result[1] = scalar_a * vector_a[1] + scalar_b * vector_b[1];
	result[2] = scalar_a * vector_a[2] + scalar_b * vector_b[2];

	return(result);
}

/*====================================================================*\
FUNCTION :	ProUtilMatrixInvert()
PURPOSE  :	Find the inverse of a transformation matrix
\*====================================================================*/
int ProUtilMatrixInvert(
	double m[4][4],
	double output[4][4])
{
	double vec[3], scale_sq, inv_sq_scale;
	int i, j;

	/*--------------------------------------------------------------------*\
	If the matrix is null, return the identity matrix
	\*--------------------------------------------------------------------*/
	if (m == NULL)
	{
		ProUtilMatrixCopy(NULL, output);
		return(1);
	}

	/*--------------------------------------------------------------------*\
	Obtain the matrix scale
	\*--------------------------------------------------------------------*/
	vec[0] = m[0][0];
	vec[1] = m[0][1];
	vec[2] = m[0][2];
	scale_sq = vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2];

	/*--------------------------------------------------------------------*\
	Check whether there is an inverse, and if not, return 0
	\*--------------------------------------------------------------------*/
	if (scale_sq < (.000000001 * .000000001))
		return(0);

	/*--------------------------------------------------------------------*\
	Need the inverse scale squared
	\*--------------------------------------------------------------------*/
	inv_sq_scale = 1.0 / scale_sq;

	/*--------------------------------------------------------------------*\
	The orientation vectors
	\*--------------------------------------------------------------------*/
	for (j = 0;j<3;j++)
	{
		for (i = 0;i<3;i++)
			output[j][i] = m[i][j] * inv_sq_scale;
		output[j][3] = 0.0;
	}

	/*--------------------------------------------------------------------*\
	The shift vectors
	\*--------------------------------------------------------------------*/
	for (i = 0;i<3;i++)
	{
		output[3][i] = 0.0;
		for (j = 0;j<3;j++)
			output[3][i] -= m[i][j] * m[3][j] * inv_sq_scale;
	}
	output[3][3] = 1.0;

	return(1);
}

/*====================================================================*\
FUNCTION :	ProUtilMatrixCopy()
PURPOSE  :	Copy one matrix to another, or initialize a matrix to
be the unitary matrix
\*====================================================================*/
void ProUtilMatrixCopy(
	double input[4][4],
	double output[4][4])
{
	int i, j;

	if (input == NULL)
	{
		for (i = 0;i<4;i++)
			for (j = 0;j<4;j++)
				output[i][j] = identity_matrix[i][j];
	}
	else
	{
		for (i = 0;i<4;i++)
			for (j = 0;j<4;j++)
				output[i][j] = input[i][j];
	}
}

/*====================================================================*\
FUNCTION :  ProUtilSketchCsysGet
PURPOSE  :  Gets the sketch csys transform
\*====================================================================*/
ProError ProUtilSketchCsysGet(
	ProTestSectionInfo *p_sketch_refs,
	double sk_transf[4][4])
{
	ProError status;

	status = ProSectionLocationGet(p_sketch_refs->section, sk_transf);

	return (PRO_TK_NO_ERROR);
}

void DrawDLine(ProSection aSec, ProTestSectionInfo *p_sketch_refs, ProMatrix transform, double xOffset,double yOffset,
	double aStartPt, double aEndPt, double fHeight,double aAngleA, double aAngleB, double(&aRealEndPt)[2] ,vector<int> & ent_ids)
{
	int ent_id;
	Pro2dLinedef line;
	ProError status;

	if (fcmp(aStartPt, aEndPt) == 0)
	{
		double aPt[2];
		ComputeTriangleStartAndEndPointByStartPt(aStartPt, fHeight, aAngleA, aAngleB, aPt);
		DrawTriangle(aSec, p_sketch_refs, transform, xOffset, yOffset, aStartPt,aPt[0],aPt[1],fHeight,ent_ids);

		aRealEndPt[0] = aStartPt;
		aRealEndPt[1] = aPt[1];

	}
	else
	{
		line.type = PRO_2D_LINE;
		line.end1[0] = aStartPt + xOffset;
		line.end1[1] = yOffset;

		ComputeTriangleStartAndEndPointByMidPt(aEndPt, fHeight, aAngleA, aAngleB, aRealEndPt);

		line.end2[0] = aRealEndPt[0] + xOffset;
		line.end2[1] = yOffset;


		if (p_sketch_refs != NULL)
		{
			ProUtil2DPointTrans(transform, line.end1, line.end1);
			ProUtil2DPointTrans(transform, line.end2, line.end2);
		}
		status = ProSectionEntityAdd(aSec, (Pro2dEntdef *)&line, &ent_id);
		ent_ids.push_back(ent_id);
	}
}

void DrawTriangle(ProSection aSec, ProTestSectionInfo *p_sketch_refs, ProMatrix transform, double xOffset,double yOffset,
	double aStartPt, double aPt, double aEndPt,double fHeight, vector<int> & ent_ids)
{
	int ent_id;
	Pro2dLinedef line;
	ProError status;

	line.type = PRO_2D_LINE;
	line.end1[0] = aStartPt+xOffset;
	line.end1[1] = yOffset;

	
	line.end2[0] = aPt+xOffset;
	line.end2[1] = fHeight+yOffset;

	if (p_sketch_refs != NULL)
	{
		ProUtil2DPointTrans(transform, line.end1, line.end1);
		ProUtil2DPointTrans(transform, line.end2, line.end2);
	}
	status = ProSectionEntityAdd(aSec, (Pro2dEntdef *)&line, &ent_id);
	ent_ids.push_back(ent_id);


	line.type = PRO_2D_LINE;
	line.end1[0] = aPt+xOffset;
	line.end1[1] = fHeight+yOffset;


	line.end2[0] = aEndPt+xOffset;
	line.end2[1] = yOffset;

	if (p_sketch_refs != NULL)
	{
		ProUtil2DPointTrans(transform, line.end1, line.end1);
		ProUtil2DPointTrans(transform, line.end2, line.end2);
	}
	status = ProSectionEntityAdd(aSec, (Pro2dEntdef *)&line, &ent_id);

	ent_ids.push_back(ent_id);
	
}

void DrawELine(ProSection aSec, ProTestSectionInfo *p_sketch_refs, ProMatrix transform, double xOffset,double yOffset,
	double aStartPt, double aEndPt, vector<int> & ent_ids)
{
	int ent_id;
	Pro2dLinedef line;
	ProError status;

	line.type = PRO_2D_LINE;
	line.end1[0] = aStartPt+ xOffset;
	line.end1[1] = yOffset;

	
	line.end2[0] = aEndPt+ xOffset;
	line.end2[1] = yOffset;


	if (p_sketch_refs != NULL)
	{
		ProUtil2DPointTrans(transform, line.end1, line.end1);
		ProUtil2DPointTrans(transform, line.end2, line.end2);
	}
	status = ProSectionEntityAdd(aSec, (Pro2dEntdef *)&line, &ent_id);

	ent_ids.push_back(ent_id);

}

void ComputeTriangleStartAndEndPointByMidPt(double aPt, double fHeight,double aAngleA, double aAngleB,double(&aRealPt)[2])
{
	aRealPt[0] = aPt;
	aRealPt[1] = aPt; 

	if (fcmp(fHeight, 0) != 0)
	{
		double fDistanceA = fHeight/tan(aAngleA);
		double fDistanceB = fHeight/tan(aAngleB);
		aRealPt[0] = aPt - fDistanceA;
		aRealPt[1] = aPt + fDistanceB;
	}
	else
	{
		aRealPt[0] = aPt;
		aRealPt[1] = aPt;
	}
}

void ComputeTriangleStartAndEndPointByStartPt(double aPt, double fHeight, double aAngleA, double aAngleB, double(&aRealPt)[2])
{
	aRealPt[0] = aPt;
	aRealPt[1] = aPt;

	if (fcmp(fHeight, 0) != 0)
	{
		double fDistanceA = fHeight/tan(aAngleA);
		double fDistanceB = fHeight/tan(aAngleB);
		aRealPt[0] = aPt + fDistanceA;
		aRealPt[1] = aRealPt[0] + fDistanceB;
	}
	else
	{
		aRealPt[0] = aPt;
		aRealPt[1] = aPt;
	}
}

void CCreateMidiCurveDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	// TODO: Add your control notification handler code here 
	UpdateData(TRUE); 

	CFileDialog    dlgFile(TRUE, NULL, NULL, OFN_HIDEREADONLY, _T("Describe Files (*.txt)|*.txt|All Files (*.*)|*.*||"), NULL);

	if (dlgFile.DoModal())
	{
		m_strPath = dlgFile.GetPathName();
	}
	UpdateData(FALSE);
}

/*====================================================================*\
FUNCTION :	ProUtilSectionClean
PURPOSE  :	Deletes all dimensions and entities from a section
\*====================================================================*/
ProError ProUtilSectionClean(ProSection section)
{
	ProError err;
	ProIntlist ent_ids, dim_ids, constr_ids;
	int n_dim_ids, n_ids, i, n_constr_ids;

#if 1
	err = ProSectionConstraintsIdsGet(section, &constr_ids, &n_constr_ids);

	for (i = 0; i<n_constr_ids; i++)
	{
		err = ProSectionConstraintDeny(section, constr_ids[i]);
		
	}

	err = ProArrayFree((ProArray *)&constr_ids);
	
#endif
	err = ProSecdimIdsGet(section, &dim_ids, &n_dim_ids);
	

	for (i = 0; i<n_dim_ids; i++)
	{
		err = ProSecdimDelete(section, dim_ids[i]);
		
	}

	err = ProArrayFree((ProArray *)&dim_ids);
	

	err = ProSectionEntityIdsGet(section, &ent_ids, &n_ids);
	

	for (i = 0; i<n_ids; i++)
	{
		err = ProSectionEntityDelete(section, ent_ids[i]);
		
	}

	err = ProArrayFree((ProArray *)&dim_ids);

	return(err);
}
