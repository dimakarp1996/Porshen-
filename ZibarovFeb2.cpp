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
	int Nr = 300;//число Ичеек по пространству равно Nr+2 - от номер 1 до номер Nr нормальные €чейки, номера 0 и Nr+1 вспомогательные
	double h = 1.00e-2;//шаг
	double t = 1e-7;//шаг по времени
	int T = 1000;//число шагов по времени
	double A = 0;//дл€ 2членной модели
	double B = 0;//дл€ нее же
	double k = 1.4;//гамма
	double SumRoU = 0, SumRoE = 0, newSumRoU = 0, newSumRoE = 0, SumFlowU, SumFlowE;//вспомогательные величины																		//константы
	double Pnorm = 101325;
	double Ronorm = 1.29;
	vector <double> U;//м/с
	vector <double> P;//ѕа
	vector <double> Ro;//кг/м3
	vector <double> E;//ƒж
	double flowleft = 0, flowright, //поток через левую и правую грань iтой €чейки
		flowUleft = 0, flowUright, //поток через левую и правую грань iтой €чейки
		flowEleft = 0, flowEright, //поток через левую и правую грань iтой €чейки
		E_Eul_left = 0, E_Eul, E_Eul_right,//энерги€ в i-1/2, i+1/2 и i+3/2 €чейке
		U_Eul_left = 0, U_Eul, U_Eul_right,//скорость в i-1/2, i+1/2 и i+3/2 €чейке
		newRo, newP, newU, newE,//новые плотность,давление,скорость,энерги€ в iтой €чейке
		newRoprev = 0, newPprev = 0, newUprev = 0, newEprev = 0,//новые плотность,давление,скорость,энерги€ в i-1той €чейке
		PUprevleft, PUleft, PUright, PUnextright,//PU в i-3/2,i-1/2, i+1/2 и i+3/2 €чейке
		Pprevleft, Pleft, Pright, Pnextright,//P в i-3/2,i-1/2, i+1/2 и i+3/2 €чейке
		uleft, u, uright,//U в i и i+1 €чейке
		roleft, ro, roright,//Ro в i и i+1 €чейке
		eleft, e, eright;//E в i и i+1 €чейке
public:
	double Energy(double P, double Ro)//находит энергию
	{
		return ((P - A) / ((k - 1)*Ro)) + B;
	}
	double Avg(double x1, double x2)
	{
		return (x1 + x2) / 2;
	}
	Solver()//«адаем начальные услови€
	{
		P.resize(Nr + 2);
		Ro.resize(Nr + 2);
		U.resize(Nr + 2);
		E.resize(Nr + 2);
		//выделили пам€ть под вектора
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
				U[i] = -379.ь15213178;
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
	void Border(bool free,bool left)//‘ункци€ дл€ гран.усл. ѕерва€ величина - лева€ ли грань, втора€ - свободна€ ли граница.
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
			if (left)//лева€ тверда€ граница
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
	void SolidBorder(bool left)//тверда€ граница
	{
		
	}
	void Record()//пишем в файл
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
	void newLayer()//главна€ функци€
	{	
		//граничные услови€
		//лева€ грань - тверда€ если leftFree= false и свободна€, если true. “о же с rightfree
		bool LeftFree = false; Border(LeftFree, true);//лева€
		bool RightFree = true; Border(RightFree, false);//права€
						  //инициализаци€ счетчиков
		SumRoU = 0; newSumRoU = 0;
		SumRoE = 0; newSumRoE = 0;
		SumFlowU = 0; SumFlowE = 0;
		//задание вспомогательных величин
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
			//Ёйлеров этап
			if (i == 1 && !LeftFree) { U_Eul_left = -U_Eul; E_Eul_left = E_Eul; flowleft = 0; flowUleft = 0; flowEleft = 0; }//г.у дл€ тверд.границы слева. ƒл€ нетвердой - считаем 	
			if (i == 1 && LeftFree) { U_Eul_left = U_Eul; E_Eul_left = E_Eul; flowleft = flowright; flowUleft = flowUright; flowEleft = flowEright; }//г.у дл€ тверд.границы слева. ƒл€ нетвердой - считаем 
			

			
			//Ћагранжев этап
			//ќпредел€ем новые значени€ параметров дл€ случаев не на границе
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
			
			if (i>1)//записываем предыдущие €чейки 
			{
				Ro[i - 1] = newRoprev;
				U[i - 1] = newUprev;
				P[i - 1] = newPprev;
				E[i - 1] = newEprev;
			}
			//“.к идет сдвиг на 1, то вычисленные значени€ Ro,P,U,E попадают в предыдущие €чейки.
			//Ќа следующем шаге они попадут в предыдущие(отн. этого следующего шага) €чейки основного массива
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
			//сдвиг U_i+1/2 на 1
			U_Eul_left = U_Eul;
			U_Eul = U_Eul_right;
			//сдвиг E_i+1/2 на 1
			E_Eul_left = E_Eul;
			E_Eul = E_Eul_right;
			//сдвиг потока на 1
			flowleft = flowright;
			flowUleft = flowUright;
			flowEleft = flowEright;
			//сдвиг P на 1
			Pprevleft = Pleft;
			Pleft = Pright;
			Pright = Pnextright;
			//сдвиг PU на 1
			PUprevleft = PUleft;
			PUleft = PUright;
			PUright = PUnextright;
			//сдвиг E на 1
			eleft = e;
			e = eright;
			//сдвиг Ro на 1
			roleft = ro;
			ro = roright;
			//сдвиг U на 1
			uleft = u;
			u = uright;
			//пишем Ro на правой границе
			if (i < Nr)//важно - они будут использоватьс€, когда i вырастет на 1
			{
				roright = Ro[i + 2];
				eright = E[i + 2];
				uright = U[i + 2];
			}
			if (i<Nr - 1)//важно - они будут использоватьс€, когда i вырастет на 1. 
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