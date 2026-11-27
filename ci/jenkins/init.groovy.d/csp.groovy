// llvm-cov가 만든 HTML 커버리지 리포트(스타일/스크립트 포함)를 Jenkins에서 그대로 볼 수 있도록
// 리포트 보기용 Content-Security-Policy를 완화한다. 로컬 개발용 Jenkins 전제이다.
// 외부에 노출하는 Jenkins라면 이 파일을 제거하고 htmlpublisher 대신 아카이브를 사용할 것.
System.setProperty(
    "hudson.model.DirectoryBrowserSupport.CSP",
    "sandbox allow-same-origin allow-scripts; default-src 'self'; " +
    "style-src 'self' 'unsafe-inline'; script-src 'self' 'unsafe-inline'; img-src 'self' data:;")
