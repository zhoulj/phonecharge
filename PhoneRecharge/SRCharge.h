#pragma once

class CSRCharge
{
public:
	int SRChargeInit();
	int Recharge(char* CardPassword,char* PhoneNum);
	CSRCharge(void);
	~CSRCharge(void);
};
