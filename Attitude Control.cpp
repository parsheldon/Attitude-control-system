#include<iostream>
#include<cmath>
#include<vector>
#include<fstream>
#include<string>
using namespace std;
//This program models the behavior of a single axis control system.
double I = 66.67; //Inertia, kg*m^2
double T = 2*110; //Torque, N*m, 2 110 N engines
//double theta; //position, degrees from positive or negative 0 to 180 or 0 to 360
//double omega; //Roll rate, degrees/sec. 
double accel = T / I; //angular acceleration
vector<double> CurrentPos;
vector<double> CurrentRate;
double DbPos = 2; //degrees
double DbRate = 0.6; //degrees/sec
double flowRate = 2 * (40.4 / 1000); //2 engine fuel mass flow rate kg/s
double totalFuelUse = 0;
int t = 0; // time elapsed in seconds
int sign(double x) //create sign function
{
	if (x > 0)
		return 1;
	else if (x < 0)
		return -1;
	else if (x == 0)
		return 0;
}

void displayVector(vector<double> Vector)
{
	for (int i = 0; i < Vector.size(); i++)
		cout << Vector[i] << " ";
}
void ControlSystem(string axis, double theta, double omega, string posfilename, string ratefilename)
{
	int coastTime = 0;
	int burnTime = 0;
	bool isNegativeAngle = false;
	if (theta < 0)
	{
		theta *= -1;
		isNegativeAngle = true;
	}
	else if (theta > 180)
	{
		theta = 360 - theta;
		isNegativeAngle = true;
	}
	CurrentPos.clear();
	CurrentRate.clear();
	CurrentPos.push_back(theta);
	CurrentRate.push_back(omega);
	for (t = 0; t < 20 * 60; t++)
	{
		double Curve = -T*sign(2 * T / I*theta + (omega*abs(theta)));
		if ((abs(theta) > DbPos) || (abs(omega) > DbRate))
		{
			if (theta > 179.49)
			{
				theta = 179.49;
			}
			else if (Curve <= 0 && (omega <= abs(1) || omega >= abs(5))) //don't burn engine if between 2 and 4 deg/s
			{
				omega -= accel;
				burnTime++;
				theta += omega;
			}
			else if (Curve > 0 && (omega <= abs(1) || omega >= abs(5)))
			{
				omega += accel;
				burnTime++;
				theta += omega;

			}
			else if ((omega >= abs(1) || omega <= abs(5)) && (abs(theta) <= DbPos))
			{
				if (Curve <= 0)
				{
					omega -= accel;
					burnTime++;
					theta += omega;
				}
				else if (Curve > 0)
				{
					omega += accel;
					burnTime++;
					theta += omega;
				}
			}
			else
			{
				theta += omega;
			}
			//theta %= 360;
			CurrentPos.push_back(theta);
			CurrentRate.push_back(omega);
		}
	}
	coastTime = CurrentPos.size() - burnTime;
	cout << "The " << axis << "-axis engines burned for a total of " << burnTime << " seconds " << endl;
	cout << "and coasted for " << coastTime << " seconds, for a total system response time of " << burnTime + coastTime << " seconds." << endl;
	cout << "The " << axis << "-axis engines used a total of " << burnTime*flowRate << " kilograms of fuel. " << endl << endl;
	totalFuelUse += burnTime*flowRate;
	ofstream PosFile;
	PosFile.open(posfilename);
	for (int i = 0; i < CurrentPos.size(); i++)
	{
		if (isNegativeAngle == false)
			PosFile << CurrentPos[i] << endl;
		else if (isNegativeAngle == true)
			PosFile << CurrentPos[i] * -1 << endl;
	}
	PosFile.close();
	ofstream RateFile;
	RateFile.open(ratefilename);
	for (int i = 0; i < CurrentRate.size(); i++)
	{
		if (isNegativeAngle == false)
			RateFile << CurrentRate[i] << endl;
		else if (isNegativeAngle == true)
			RateFile << CurrentRate[i] * -1 << endl;
	}

	RateFile.close();
}
int main()
{
	ControlSystem("x", 180, 0, "xpositionvector.txt", "xratevector.txt");
	ControlSystem("y", 270, 0, "ypositionvector.txt", "yratevector.txt");
	ControlSystem("z", -90, 0, "zpositionvector.txt", "zratevector.txt");
	cout << "The reaction control system used a combined total of " << totalFuelUse << " kilograms of fuel." << endl;
	
	
	return 0;
}