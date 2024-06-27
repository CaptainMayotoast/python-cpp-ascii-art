## Notes

- Third-party libraries that are header only are wrapped in another dependency, because header only dependencies are compiled directly into a library or executable.  There is no other way I know of to elide header only dependency warnings.