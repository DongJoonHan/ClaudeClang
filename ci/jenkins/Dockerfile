# Jenkins 안정화(LTS) + Docker CLI + 필요한 플러그인(설치 완료) + JCasC 설정
# 빌드 작업은 이 컨테이너가 아니라 claudeclangbuild 이미지에서 실행한다 (Jenkinsfile 참고).
FROM jenkins/jenkins:lts-jdk21

USER root

RUN apt-get update \
 && apt-get install -y --no-install-recommends ca-certificates curl gnupg git \
 && install -m 0755 -d /etc/apt/keyrings \
 && curl -fsSL https://download.docker.com/linux/debian/gpg -o /etc/apt/keyrings/docker.asc \
 && echo "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/docker.asc] https://download.docker.com/linux/debian $(. /etc/os-release && echo "$VERSION_CODENAME") stable" \
        > /etc/apt/sources.list.d/docker.list \
 && apt-get update \
 && apt-get install -y --no-install-recommends docker-ce-cli \
 && rm -rf /var/lib/apt/lists/* \
 && git config --system --add safe.directory '*'

COPY plugins.txt /usr/share/jenkins/ref/plugins.txt
RUN jenkins-plugin-cli --plugin-file /usr/share/jenkins/ref/plugins.txt

COPY casc/ /usr/share/jenkins/casc_configs/
COPY init.groovy.d/ /usr/share/jenkins/ref/init.groovy.d/

ENV CASC_JENKINS_CONFIG=/usr/share/jenkins/casc_configs \
    JAVA_OPTS="-Djenkins.install.runSetupWizard=false"
