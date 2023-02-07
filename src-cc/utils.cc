#include <iostream>
#include <fstream>
#include "utils.h"

string itos(int arg) {
  ostringstream buffer;
  buffer << arg;
  return buffer.str();
}

string ftos(float arg) {
  ostringstream buffer;
  buffer << arg;
  return buffer.str();
}

string dtos(double arg) {
  ostringstream buffer;
  buffer << arg;
  return buffer.str();
}

void Clamp(double &arg, double min, double max) {
  if(arg<min)
    arg = min;
  if(arg>max)
    arg = max;
}

bool WriteOutput(const char* fileName, vector<double> toWrite, bool overwrite) {
  FILE* output;
  char* option;
  if(overwrite)
    option = "w";
  else
    option = "a";
  if((output=fopen(fileName,option))==NULL) {
    cout << "WriteOutput Error(2)!!" << endl;
  }
  for(int i=0;i<(int)toWrite.size();++i) {
    fprintf(output, "%f ", toWrite[i]);
  }
  fprintf(output, "\n");

  fclose(output);
  
  return true;
}

bool WriteOutput(const char* fileName, int toWrite) {
  FILE* output;
  if((output=fopen(fileName, "a"))==NULL) {
    cout << "WriteOutput Error(3)!!" << endl;
    return false;
  }
  fprintf(output, "%d ", toWrite);
  fprintf(output, "\n");

  fclose(output);

  return true;
}

bool WriteOutput(const char* fileName, double toWrite) {
  FILE* output;
  if((output=fopen(fileName, "a"))==NULL) {
    cout << "WriteOutput Error(4)!!" << endl;
    return false;
  }
  fprintf(output, "%f ", toWrite);
  fprintf(output, "\n");

  fclose(output);

  return true;
}

bool WriteOutput(const char* fileName, vector<doubleVec> toWrite) {
  FILE* output;
  if((output=fopen(fileName, "w"))==NULL) {
    cout << "WriteOutput Error(5)!!" << endl;
    return false;
  }
  for(int i=0;i<(int)toWrite.size();++i) {
    for(int j=0;j<(int)toWrite[i].size();++j) {
      fprintf(output, "%f ", toWrite[i][j]);
    }
    fprintf(output, "\n");
  }
  fclose(output);

  return true;
}

bool WriteOutput(const char* fileName, vector<intVec> toWrite) {
  FILE* output;
  if((output=fopen(fileName, "w"))==NULL) {
    cout << "WriteOutput Error(6)!!" << endl;
    return false;
  }
  for(int i=0;i<(int)toWrite.size();++i) {
    for(int j=0;j<(int)toWrite[i].size();++j) {
      fprintf(output, "%d ", toWrite[i][j]);
    }
    fprintf(output, "\n");
  }
  fclose(output);

  return true;
}

double to_double(const char* p) {
  std::stringstream ss(p);
  double result = 0;
  ss >> result;
  return result;
}

vector<double> ReadFile() {
  int len;
  char token[32];
  vector<double> toReturn;

  ifstream in_stream("weights.dat", ifstream::in);

  if(!in_stream.is_open()) {
    cout << "ReadFile Error!!" << endl;
    exit(1);
  }

  while(in_stream >> token) {
    len = (int)strlen(token);
    if(token[len-1]==' ')
      token[len-1] = '\0';
    toReturn.push_back(strtod(token, NULL));
  }
  in_stream.close();

  return toReturn;
}

bool inTheVec(vector<int> intVec, int val) {
	for(int i=0;i<(int)intVec.size();++i) {
		if(intVec[i]==val)
			return true;
	}

	return false;
}

bool inTheVec(vector<double> doubleVec, double val) {
	for(int i=0;i<(int)doubleVec.size();++i) {
		if(doubleVec[i]==val)
			return true;
	}

	return false;
}