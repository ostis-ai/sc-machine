## Using sc-machine inside Docker

All our releases are automatically uploaded to Docker Hub, so to start using our latest release it's enough to [have Docker installed and configured](https://docs.docker.com/get-started/) (**please note** that Docker from `snap` and Debian's/Ubuntu's repo are known to be broken, install using the official guide).

### Launch

To pull the sc-machine image from DockerHub and run it, follow these steps:

```sh
# build knowledge base (see below for details)
docker compose run --rm machine build
# launch machine
docker compose up
```

Generally you would want to use a knowledge base source folder alongside sc-machine. To do that, create a "kb" folder in the root of the project and place the KB sources in it. After that build it using the command described above (or enable autorebuild as shown below).

Note: By default we expect you to place a [repo.path](sc-tools/kb_repo_file.md) file inside the `./kb` folder, but in case you don't have one you can configure to build the folder itself by modifying the `.env` file:

```diff
+KB_PATH="/kb"
```

If you want to auto-rebuild the knowledge base on sc-machine restart, you can also configure this behavior in the `.env` file:

```diff
+REBUILD_KB=1
```

## docker_entrypoint.sh

Our Docker entrypoint script has two commands: `build` and `run`. The former is used to build or update knowledge base, and the latter is used to launch `sc-machine`. You can use it in your own projects that use sc-machine as the entrypoint. Don't forget to configure custom binary and knowledge base paths. Consult with the `docker-entrypoint.sh --help` for the full list of available flags.

## Rebuild image

In case you want to run your own, changed version of `sc-machine` or you made some contributions that require changes to the image, use our `Dockerfile` to build a new version of the image. To rebuild the image, launch `docker build . -t ostis/sc-machine` in the root folder of this project. You'll be able to launch it using `docker compose up` afterwards.
