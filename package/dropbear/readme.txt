1. use Linux's ssh-keygen utility to generate private and public keys
   => ssh-keygen -C [username]
	  ssh-keygen -C root
      ssh-keygen will create two files(id_rsa and id_rsa.pub).
   => id_rsa:
-----BEGIN RSA PRIVATE KEY-----
MIIEpAIBAAKCAQEAtzk5vvFaqtre5jRuzspFfIcZUuuqkBqpmzgI2+lvhFHT3KCX
aQ10LgyjZ1qWQj2TDuVG9Fy4Bs/qT8Zdy3iX/Vc+9f0YFPLi+BJcq/zL+IawFcjj
0UMqtu8mIXzCkPWPojjiY5qRu0ZsYM55CESnf318HAaFQQwENK3bL/GS3wkmyel4
3xA32O39LWt3n3V2o8XAVg5jybFzhrTyIuzrYkWfL/HLBxGSbp59P4nZ9JCxqdPx
e2TWE0I4fx7KMFF0qwAGNBruqYpPu2m3Ku6B1JDNfk9eY+rYmYr/79QPN4KDw2Bx
N2wZV6sPlejU6jQL0izADEmUfbEb7zOEQxeWUQIDAQABAoIBAQC2+6zDGDFvjrSr
fVPoq7OnwRi2amvmDOqz/MtELSbVJA+NOMjd/rS3xKFwd9S2hN4YMzygkPSswPqF
/1ru+Rn5xic37jMdwNCR5WytywM9ye2TZBS5ifXfyojvSrTZppRy85KM3PKL8LIO
IEZZzo7lUUIgjxaHtuOAZN0RTZZbrQxQUlHpcniGLXJxmza1pP6z3wDV9IcuUYHm
RGphvAf3XQVvI/4z98A4J8kHX4tyiHYejG0T+5KLmLs6q5Lh6bA6X54MS+vPjHsj
1+3jvhiuNJonOXuPmOgED0lhsoN+xnUMmR9M/Jj/+PZ7HiY/UwTn3CivcYOjO/IE
PyH3D6IBAoGBAN67ZSlkFx3f2n6QtXD+/HJFYDmtvWEOFJTBUPTDuHXdJzG3ROrF
r+Wsbv1w5r5lyv45qWLzteVB0mU+CW3joT+/tUDg6NTnLQ84JEDVQP/dJu/eugRK
AQnEs/AFbG+wHKuNJZ3veGNtYeTTC67lXY3z87r6mf/jPnFL8N5PEF4hAoGBANKX
JQqdVyE41bF4i1wlAlGcLVVK96btSKyY9OvYPDtmbT/YXTh2tW1hPE3f5kQJImuS
S1fKGVeeZppdyxcWOI0MffLlaa4bJnCf6+fds6+YQv1bSwqkc1biDCUZdFmP4LUu
ixoJVgWSKfHw6s8wiLvR6d4+MSiL0jodtSfC71IxAoGAFXt7kVdvB1mc6FIqveBK
FvdsBYQlXOQPxh0zIrhx5SRUcG4dvoscoQlVlyo5hymoP4buOHYLGDaWBV/LZwbq
eYyS/dPzgP1fEjW3AW3QHB2HcOFJjbpZiUiolkDfqZIR4dX2R9peATD+nSRh3xeO
Q2/BFUqwacbQMftCGx8DFCECgYBL7VzYl+QLQV+irhCo91r/q6yYnP/veowCVt+v
jPYigIqRQ2RQ8FP3jOBeP4TZMOXtHQD3NRGwCpd5vqlzuHpJOKDMi7RvprBUWQSI
Q+H+RIqaqGjE/IftYnD4i1Xs2Lgu+woCKC2QsqNW7QLsEtapyURl8cxtjwQMGhvv
qxP4cQKBgQDNArlUF1kOQq6CcIsNxbkSSlXeVVg/axwkGnc9XgewF1zpYwNAM9uY
Br4EXFsBXmYNIsiHz81v0OPuWnjI4NgmnECoUReViacsdrwRhpIcmyNzZfdVQc8G
RjfTCAi7XVCdeSTqZhh+mXFXUTuPExfTpvGjSEFWX9udAH2mfCZV+Q==
-----END RSA PRIVATE KEY-----

   => id_rsa.pub
ssh-rsa AAAAB3NzaC1yc2EAAAADAQABAAABAQC3OTm+8Vqq2t7mNG7OykV8hxlS66qQGqmbOAjb6W+EUdPcoJdpDXQuDKNnWpZCPZMO5Ub0XLgGz+pPxl3LeJf9Vz71/RgU8uL4Elyr/Mv4hrAVyOPRQyq27yYhfMKQ9Y+iOOJjmpG7RmxgznkIRKd/fXwcBoVBDAQ0rdsv8ZLfCSbJ6XjfEDfY7f0ta3efdXajxcBWDmPJsXOGtPIi7OtiRZ8v8csHEZJunn0/idn0kLGp0/F7ZNYTQjh/HsowUXSrAAY0Gu6pik+7abcq7oHUkM1+T15j6tiZiv/v1A83goPDYHE3bBlXqw+V6NTqNAvSLMAMSZR9sRvvM4RDF5ZR bitonic

2. If we want to use putty ssh client to connect to OpenSSH or Dropbear, we must use putty key generation utility to load id_rsa file.
   Then use this utility to generate private for putty utility.
   Now, we can start to config putty to use Public key method.
   => Putty config:
      Connection->Data->Auto-login username: root
      Connection->SSH->Auth->Private key file for authentication: putty's private key

3. Openwrt Dropbear config:
   => /etc/config/dropbear
config dropbear
        option PasswordAuth 'off'
        option Port         '22'
        option BannerFile   '/etc/banner'
   => /etc/dropbear directory
   	  /etc/dropbear is linked to /etc/config/dropbear_key
      This directory must has permission 700.
   => /etc/dropbear/authorized_keys file
      This file must has permission 600.
      We can put public key lists to authorized_keys file.
ssh-rsa AAAAB3NzaC1yc2EAAAADAQABAAABAQC3OTm+8Vqq2t7mNG7OykV8hxlS66qQGqmbOAjb6W+EUdPcoJdpDXQuDKNnWpZCPZMO5Ub0XLgGz+pPxl3LeJf9Vz71/RgU8uL4Elyr/Mv4hrAVyOPRQyq27yYhfMKQ9Y+iOOJjmpG7RmxgznkIRKd/fXwcBoVBDAQ0rdsv8ZLfCSbJ6XjfEDfY7f0ta3efdXajxcBWDmPJsXOGtPIi7OtiRZ8v8csHEZJunn0/idn0kLGp0/F7ZNYTQjh/HsowUXSrAAY0Gu6pik+7abcq7oHUkM1+T15j6tiZiv/v1A83goPDYHE3bBlXqw+V6NTqNAvSLMAMSZR9sRvvM4RDF5ZR root
   => one root with multiple public keys
   	  all public keys can be appended to /etc/dropbear/authorized_keys file.
	=> multiple users
	   a) openwrt must create /home directory for all users
	      /home/bitonic
	   b) /etc/passwd
	      bitonic:$1$ez1bqUIp$2Eqtj.0ts49Q6s/0bHhdt1:1002:1002:bitonic:/home/bitonic:/bin/ash
	   c) /etc/group
	      bitonic:x:1002:
	   d) create .ssh directory in home directory
	      /home/bitonic/.ssh
	   e) add authorized_keys file to /home/bitonic/.ssh/authorized_keys and append bitonic's public key to authorized_keys file

4. some prepared OpenSSH keys are stored in keys directory

5. dropbear package default directories:
   a)
	/* Default hostkey paths - these can be specified on the command line */
	#ifndef DSS_PRIV_FILENAME
	#define DSS_PRIV_FILENAME "/etc/dropbear/dropbear_dss_host_key"
	#endif
	#ifndef RSA_PRIV_FILENAME
	#define RSA_PRIV_FILENAME "/etc/dropbear/dropbear_rsa_host_key"
	#endif

   b)
	if (ses.authstate.pw_uid != 0) {
		/* we don't need to check pw and pw_dir for validity, since
		 * its been done in checkpubkeyperms. */
		len = strlen(ses.authstate.pw_dir);
		/* allocate max required pathname storage,
		 * = path + "/.ssh/authorized_keys" + '\0' = pathlen + 22 */
		filename = m_malloc(len + 22);
		snprintf(filename, len + 22, "%s/.ssh/authorized_keys",
		         ses.authstate.pw_dir);

		/* open the file */
		authfile = fopen(filename, "r");
	} else {
		authfile = fopen("/etc/dropbear/authorized_keys","r");
	}