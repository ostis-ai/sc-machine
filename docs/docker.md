## Using sc-machine inside Docker

All our releases are automatically uploaded to Docker Hub, so to start using our latest release it's enough to [have Docker installed and configured](https://docs.docker.com/get-started/) (**please note** that Docker from `snap` and Debian's/Ubuntu's repo are known to be broken, install using the official guide).

To launch our image, you'll have to change the KB folder mounted in `docker-compose.yml` file (or put existing files in ./kb folder in the root of the project):

```diff
    volumes:
-      - ./kb:/kb
+      - <path/to/your/kb>:/kb
      - kb-binary:/kb.bin
    environment:
      # Use the commented env variable if you need to rebuild KB every startup.
-      #- "REBUILD_KB=1"
+      - "REBUILD_KB=1"
-      - "KB_PATH=/kb/repo.path"
      # Change this if you don't have repo.path in your KB
+      - "KB_PATH=/kb"

```

```sh
docker compose run machine build #build KB
docker compose up #launch server
```

## Working with knowledge base in Docker

We're using `./kb` as the default location for the KB files. KB location can be changed in `./docker-compose.yml`
By default, KB rebuild is disabled, so you'll have to rebuild it manually. You can uncomment `KB_REBUILD=1` env variable in `docker-compose.yml` in case you need to rebuild KB on each relaunch. Running `docker compose run machine rebuild` is unnecessary in this case.

## Entrypoint

Our Docker image has two commands: `build` and `serve`. The former is used to build or update knowledge base, and the latter is used to launch `sc-server`.

## Building KBs using Docker image

One thing to note is that **we do not build KB on startup by default**.

You can override this behavior in one of the following ways:
Define `$REBUILD_KB` environment variable by adding `-e REBUILD_KB=1` to the `docker run` arguments. In this case, launching the image in `serve` mode will rebuild the KB automatically.

You may also want to change the path to KB being built in case you mounted it in a different location (or if you're using a repo file). To do this, add `-e KB_PATH=</your/path>`.

Another way to rebuild the KB without launching the server is to launch it in `build` mode. To do this, simply substitute `serve` by `build` in the command shown in [Using sc-machine inside Docker](#using-sc-machine-inside-docker) section.

## Rebuild image

In case you want to run your own, customized version of `sc-machine` or you made some contributions that require changes to the image, use our `Dockerfile` to build a new version of the image. To rebuild the image, launch `docker build . -t ostis/sc-machine` in the root folder of this project.
