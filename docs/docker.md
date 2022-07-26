## Using sc-machine inside Docker
All our releases are automatically uploaded to Docker Hub, so to start using our latest release it's enough to [have Docker installed and configured](https://docs.docker.com/get-started/) (**please note** that Docker from `snap` and Debian's/Ubuntu's repo are known to be broken, install using the official guide).

To launch our image, use this command: `docker run --rm -i -t -v <path to knowledge base>:/kb -v kb_binary:/kb.bin ostis/sc-machine:latest`

## Building KBs with repo.path inside Docker
One thing to note is that **we don't use `repo.path` file inside the KB folder in the default launch command**. 

In case your KB depends on it, you might want to change the entrypoint to `/bin/bash` and use scripts to do it manually: `docker run --rm -i -t --entrypoint "/bin/bash" -v <path to kb>:/kb -v kb_binary:/kb.bin ostis/sc-machine:latest`


## Rebuild image
In case you want to run your own, customized version of `sc-machine` or you made some contributions that require changes to the image, use our `Dockerfile` to build a new version of the image. To build image launch `docker build . -t ostis/sc-machine` in the root folder of this project.