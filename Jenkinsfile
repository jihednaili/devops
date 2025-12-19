pipeline {
    agent any

    environment {
        REGISTRY = "midenimeed"
        REGISTRY_CREDENTIALS_ID = "dockerhub-creds"
        IMAGE_TAG = "build-${env.BUILD_NUMBER}"
    }

    stages {
        stage('Checkout') {
            steps {
                checkout scm
            }
        }

        stage('Docker Build') {
            steps {
                sh 'docker build -t ${REGISTRY}/serveur:${IMAGE_TAG} serveur'
                sh 'docker build -t ${REGISTRY}/client:${IMAGE_TAG} client'
            }
        }

        stage('Docker Push') {
            steps {
                withCredentials([usernamePassword(credentialsId: env.REGISTRY_CREDENTIALS_ID, usernameVariable: 'DOCKER_USER', passwordVariable: 'DOCKER_PASS')]) {
                    sh 'echo "$DOCKER_PASS" | docker login -u "$DOCKER_USER" --password-stdin'
                    sh 'docker push ${REGISTRY}/serveur:${IMAGE_TAG}'
                    sh 'docker push ${REGISTRY}/client:${IMAGE_TAG}'
                }
            }
        }

        stage('Deploy to K8s') {
            steps {
                sh 'kubectl get nodes'
                sh '''
                mkdir -p .jenkins-tmp
                sed "s|YOUR_REGISTRY/serveur:latest|${REGISTRY}/serveur:${IMAGE_TAG}|" ci-cd-config/k8s-serveur-deployment.yaml > .jenkins-tmp/k8s-serveur.yaml
                sed "s|YOUR_REGISTRY/client:latest|${REGISTRY}/client:${IMAGE_TAG}|" ci-cd-config/k8s-client-deployment.yaml > .jenkins-tmp/k8s-client.yaml
                kubectl apply -f .jenkins-tmp/k8s-serveur.yaml
                kubectl apply -f .jenkins-tmp/k8s-client.yaml
                '''
            }
        }
    }
}
