# ldap authentication using Golang

Binary location: https://sdartifact.td.teradata.com:443/artifactory/pkgs-generic-qa-sd/ppi/ldaps

Using LDAPUSER and LDAPPASS environment variables will bypass the user/password prompts

```
#ldaps
Usage: ldaps /path/to/config-file
      ------Sample-----
# client ldap configuration file
server = ldap-sd.td.teradata.com
port = 389
service account = cn=rh122058,ou=application accounts,dc=td,dc=teradata,dc=com
service password = secret-password
```
