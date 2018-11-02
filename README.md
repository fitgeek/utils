# Utilities
## rpmbuild automation
### Automatically create a %install spec section for rpmbuild
Remove the tedious spec file editing chore of adding information for each file within an rpm.  
```
# ./createinstallspec /full/path/to/directory >capture_output.txt
```
I churned this out by copying what others place in their spec files. I am definitely not a master rpm builder :)
Improvements that could be made are pruning git files and tmp files.

Tested by building python2.7 and packaging it.

Comments are very much welcome.
