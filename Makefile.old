ifeq ($(colors),off)
DEV_SH_ENV := NON_INTERACTIVE=1 MONO=1
else
DEV_SH_ENV := NON_INTERACTIVE=1
endif

docker-run: .make/koiz-dev
	scripts/docker/dev-sh "./run.sh"

clean:
	rm -rf .make
	docker image rm koiz-dev:latest || echo "failed to remove image"
	docker system prune -af

.make/cache:
	mkdir -p .make
	touch .make/cache

.make/koiz-dev: .make/cache ./Dockerfile ./scripts/docker/*
	docker build --network host -f ./Dockerfile -t koiz-dev:latest .
	touch .make/koiz-dev
