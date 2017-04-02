//Note:The template file will be copied to a new file. When you change the code of the template file you can create new file with this base code.
#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <string>
#include <locale>

using namespace std;
class Solver
{
	int Nr = 300;//����� ����� �� ������������ ����� Nr+2 - �� ����� 1 �� ����� Nr ���������� ������, ������ 0 � Nr+1 ���������������
	double h = 1.00e-2;//���
	double t = 1e-7;//��� �� �������
	int T = 1000;//����� ����� �� �������
	double A = 0;//��� 2������� ������
	double B = 0;//��� ��� ��
	double k = 1.4;//�����
	double SumRoU = 0, SumRoE = 0, newSumRoU = 0, newSumRoE = 0, SumFlowU, SumFlowE;//��������������� ��������																		//���������
	double Pnorm = 101325;
	double Ronorm = 1.29;
	vector <double> U;//�/�
	vector <double> P;//��
	vector <double> Ro;//��/�3
	vector <double> E;//��
	double flowleft = 0, flowright, //����� ����� ����� � ������ ����� i��� ������
		flowUleft = 0, flowUright, //����� ����� ����� � ������ ����� i��� ������
		flowEleft = 0, flowEright, //����� ����� ����� � ������ ����� i��� ������
		E_Eul_left = 0, E_Eul, E_Eul_right,//������� � i-1/2, i+1/2 � i+3/2 ������
		U_Eul_left = 0, U_Eul, U_Eul_right,//�������� � i-1/2, i+1/2 � i+3/2 ������
		newRo, newP, newU, newE,//����� ���������,��������,��������,������� � i��� ������
		newRoprev = 0, newPprev = 0, newUprev = 0, newEprev = 0,//����� ���������,��������,��������,������� � i-1��� ������
		PUprevleft, PUleft, PUright, PUnextright,//PU � i-3/2,i-1/2, i+1/2 � i+3/2 ������
		Pprevleft, Pleft, Pright, Pnextright,//P � i-3/2,i-1/2, i+1/2 � i+3/2 ������
		uleft, u, uright,//U � i � i+1 ������
		roleft, ro, roright,//Ro � i � i+1 ������
		eleft, e, eright;//E � i � i+1 ������
public:
	double Energy(double P, double Ro)//������� �������
	{
		return ((P - A) / ((k - 1)*Ro)) + B;
	}
	double Avg(double x1, double x2)
	{
		return (x1 + x2) / 2;
	}
	Solver()//������ ��������� �������
	{
		P.resize(Nr + 2);
		Ro.resize(Nr + 2);
		U.resize(Nr + 2);
		E.resize(Nr + 2);
		//�������� ������ ��� �������
		printf("%d", P.size());

		for (int i = 1; i < Nr + 1; i++)
		{
#if 0
			if (i == 1)
			{
				P[i] = Pnorm * 100;		
			}
			else
			{
				P[i] = Pnorm*(1 + i);
			}
			Ro[i] = Ronorm*(1+i);
			U[i] = 100*i;
			E[i] = Energy(P[i], Ro[i]);
#endif
#if 1
			if (i <Nr / 2)
			{
				P[i] = 42.71033803 * Pnorm;
				Ro[i] = 6.8046564525;
				U[i] = -379.�15213178;
			}
			else
			{
				P[i] = Pnorm;
				Ro[i] = Ronorm;
				U[i] = -2000;
			}
			E[i] = Energy(P[i], Ro[i]);
#endif
#if 0
			if (i ==1)
			{
				P[i] =100* Pnorm;
			}
			else
			{
				P[i] = Pnorm;
				
			}
			Ro[i] = Ronorm;
			U[i] = 0;
			E[i] = Energy(P[i], Ro[i]);
#endif
		}

	}
	void Border(bool free,bool left)//������� ��� ����.���. ������ �������� - ����� �� �����, ������ - ��������� �� �������.
	{
		if (free)
		{
			if (left)
			{
				Ro[0] = Ro[1];
				U[0] = U[1];
				P[0] = P[1];
				E[0] = E[1];
			}
			if(!left)
			{
				Ro[Nr + 1] = Ro[Nr];
				U[Nr + 1] = U[Nr];
				P[Nr + 1] = P[Nr];
				E[Nr + 1] = E[Nr];
			}
		}
		if (!free)
		{
			if (left)//����� ������� �������
			{
				Ro[0] = Ro[1];
				U[0] = -U[1];
				P[0] = P[1];
				E[0] = E[1];
			}
			else
			{
				Ro[Nr + 1] = Ro[Nr];
				U[Nr + 1] = -U[Nr];
				P[Nr + 1] = P[Nr];
				E[Nr + 1] = E[Nr];
			}
		}
	}
	void SolidBorder(bool left)//������� �������
	{
		
	}
	void Record()//����� � ����
	{
		double time = (T)*t;
		double length = Nr*h;
		ofstream fout;
		string C = "Time-" + std::to_string(time) + "sec-" + "Length-" + std::to_string(length) + "m.csv";
		fout.open(C);
		fout << "X;" << "Ro;" << "U;" << "P;" << "E;" << "RoU;" << "RoE;" << endl;
		cout << "X  " << "Ro  " << "U  " << "P  " << "E " << "RoU " << "RoE " << endl;
		string F;
		double SumRo = 0;
		for (int i = 1; i <= Nr; i++)
		{
			F = std::to_string(i*h) + ";" + std::to_string(Ro[i]) + ";" + std::to_string(U[i]) + ";" + std::to_string(P[i] / Pnorm) + ";" + std::to_string(E[i]) + ";" + std::to_string(Ro[i] * U[i]) + ";" + std::to_string(Ro[i] * E[i]);
			std::replace(F.begin(), F.end(), '.', ',');
			fout << F << endl;
			std::replace(F.begin(), F.end(), ';', ' ');
			cout << F << endl;
			SumRo += Ro[i];
		}
		cout << "Sum of Ro is " << SumRo << endl;
		//cout << "Sum of RoU is " << SumRoU << endl; cout << " Sum of RoU on next step is " << newSumRoU << endl;
		//cout << "Sum of RoE is " << SumRoE << endl; cout << " Sum of RoE on next step is " << newSumRoE << endl;
		cout << "Sum of flow of U " << SumFlowU << endl;
		cout << "Sum of flow of E " << SumFlowE << endl;
		fout.close();

	}
	void newLayer()//������� �������
	{	
		//��������� �������
		//����� ����� - ������� ���� leftFree= false � ���������, ���� true. �� �� � rightfree
		bool LeftFree = false; Border(LeftFree, true);//�����
		bool RightFree = true; Border(RightFree, false);//������
						  //������������� ���������
		SumRoU = 0; newSumRoU = 0;
		SumRoE = 0; newSumRoE = 0;
		SumFlowU = 0; SumFlowE = 0;
		//������� ��������������� �������
		Pleft = Avg(P[0], P[1]);
		Pright = Avg(P[1], P[2]); Pnextright = Avg(P[2], P[3]);
		PUleft = Avg(P[0] * U[0], P[1] * U[1]);
		PUright = Avg(P[1] * U[1], P[2] * U[2]); PUnextright = Avg(P[2] * U[2], P[3] * U[3]);
		u = U[1]; uright = U[2]; uleft = U[0];
		e = E[1]; eright = E[2]; eleft = E[0];
		ro = Ro[1]; roright = Ro[2]; roleft = Ro[0];
		for (int i = 1; i <= Nr; i++)
		{
			if (i == 1)
			{
				U_Eul = u - ((Pright - Pleft)*t / (h*ro));
				E_Eul = e - ((PUright - PUleft)*t / (h*ro));
			}
			U_Eul_right = uright - ((Pnextright - Pright)*t / (h*roright));
			E_Eul_right = eright - ((PUnextright - PUright)*t / (h*roright));
			
			if (i == Nr && RightFree) { U_Eul_right = U_Eul; E_Eul_right = E_Eul; flowright = flowleft; flowUright = flowUleft; flowEright = flowEleft; }
			if (i == Nr && !RightFree) { U_Eul_right = -U_Eul; E_Eul_right = E_Eul; flowright = 0; flowUright = 0; flowEright = 0; }
			if ((U_Eul + U_Eul_right > 0))
			{
				flowright = Avg(U_Eul, U_Eul_right)*ro * t;
				flowEright = Avg(U_Eul, U_Eul_right)*ro *E_Eul* t;
				flowUright = Avg(U_Eul, U_Eul_right)*ro *U_Eul* t;
			}
			else
			{
				flowright = Avg(U_Eul_right, U_Eul)*roright* t;
				flowEright = Avg(U_Eul_right, U_Eul)*roright*E_Eul_right*t;
				flowUright = Avg(U_Eul_right, U_Eul)*roright*U_Eul_right*t;
			}
			//������� ����
			if (i == 1 && !LeftFree) { U_Eul_left = -U_Eul; E_Eul_left = E_Eul; flowleft = 0; flowUleft = 0; flowEleft = 0; }//�.� ��� �����.������� �����. ��� ��������� - ������� 	
			if (i == 1 && LeftFree) { U_Eul_left = U_Eul; E_Eul_left = E_Eul; flowleft = flowright; flowUleft = flowUright; flowEleft = flowEright; }//�.� ��� �����.������� �����. ��� ��������� - ������� 
			

			
			//��������� ����
			//���������� ����� �������� ���������� ��� ������� �� �� �������
#if 0
			cout << " flowleft " << flowleft << " flowright " << flowright;
			cout << " flowUleft " << flowUleft << " flowUright " << flowUright;
			cout << " flowEleft " << flowEleft << " flowEright " << flowEright << endl;
#endif
#if 0
			cout << " flow "  << flowright-flowleft;
			cout << " flowUleft " << flowUright- flowUleft;
			cout << " flowEleft " << flowEright- flowEleft << endl;
#endif
			
			
				newRo = ro + (flowleft - flowright) / h;
				newU = (U_Eul * ro *h + flowUleft - flowUright) / (h*newRo);
				newE = (E_Eul * ro *h + flowEleft - flowEright) / (h*newRo);
				newP = ((newE - B)*newRo * (k - 1)) + A;
				SumRoU += Ro[i] * U_Eul; SumRoE += Ro[i] * E_Eul;
				newSumRoU += newRo*newU; newSumRoE += newRo*newE;
				SumFlowU += flowUleft - flowUright; SumFlowE += flowEleft - flowEright;
			
			if (i>1)//���������� ���������� ������ 
			{
				Ro[i - 1] = newRoprev;
				U[i - 1] = newUprev;
				P[i - 1] = newPprev;
				E[i - 1] = newEprev;
			}
			//�.� ���� ����� �� 1, �� ����������� �������� Ro,P,U,E �������� � ���������� ������.
			//�� ��������� ���� ��� ������� � ����������(���. ����� ���������� ����) ������ ��������� �������
#if 0
			if (i == Nr && RightFree)
			{
				Ro[i] = Ro[i-1];
				U[i] = U[i-1];
				E[i] = E[i-1];
				P[i] = P[i-1];
			}
			if (i == 2 && LeftFree)
			{
				Ro[i-1] = Ro[i];
				U[i-1] = U[i];
				E[i-1] = E[i];
				P[i-1] = P[i];
			}
#endif
			newRoprev = newRo;
			newUprev = newU;
			newPprev = newP;
			newEprev = newE;
			//����� U_i+1/2 �� 1
			U_Eul_left = U_Eul;
			U_Eul = U_Eul_right;
			//����� E_i+1/2 �� 1
			E_Eul_left = E_Eul;
			E_Eul = E_Eul_right;
			//����� ������ �� 1
			flowleft = flowright;
			flowUleft = flowUright;
			flowEleft = flowEright;
			//����� P �� 1
			Pprevleft = Pleft;
			Pleft = Pright;
			Pright = Pnextright;
			//����� PU �� 1
			PUprevleft = PUleft;
			PUleft = PUright;
			PUright = PUnextright;
			//����� E �� 1
			eleft = e;
			e = eright;
			//����� Ro �� 1
			roleft = ro;
			ro = roright;
			//����� U �� 1
			uleft = u;
			u = uright;
			//����� Ro �� ������ �������
			if (i < Nr)//����� - ��� ����� ��������������, ����� i �������� �� 1
			{
				roright = Ro[i + 2];
				eright = E[i + 2];
				uright = U[i + 2];
			}
			if (i<Nr - 1)//����� - ��� ����� ��������������, ����� i �������� �� 1. 
			{
				PUnextright = Avg(P[i + 2] * U[i + 2], P[i + 3] * U[i + 3]);
				Pnextright = Avg(P[i + 2], P[i + 3]);
			}
			if (i == Nr - 1)
			{
				PUnextright = P[i + 2] * U[i + 2];
				Pnextright = P[i + 2];
			}
		}

	}


	void Solve()
	{
		Record();
		for (int i = 0; i < T; i++)
		{
			newLayer();
			
		}
		Record();

	}
};

int main(void)
{
	Solver A;
	A.Solve();
}