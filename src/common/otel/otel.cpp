//
// Created by peter wang on 2021/12/22.
//

#include "otel.h"

#include <opentelemetry/sdk/trace/simple_processor.h>
#include <opentelemetry/sdk/trace/tracer_provider.h>
#include <opentelemetry/trace/provider.h>
#include <opentelemetry/exporters/otlp/jaeger_exporter.h>

namespace sdktrace = opentelemetry::sdk::trace;
using namespace opentelemetry::exporter::jaeger;

void setUpTracer(bool inCompose)
{
  OtlpGrpcExporterOptions options;
  options.endpoint = "localhost:12345";
  auto exporter = std::unique_ptr<sdktrace::SpanExporter>(new otlp::OtlpGrpcExporter(options));

  auto processor = std::unique_ptr<sdktrace::SpanProcessor>(
      new sdktrace::SimpleSpanProcessor(std::move(exporter)));

  auto provider = opentelemetry::nostd::shared_ptr<opentelemetry::trace::TracerProvider>(
      new sdktrace::TracerProvider(std::move(processor)));

  opentelemetry::trace::Provider::SetTracerProvider(provider);
}

opentelemetry::nostd::shared_ptr<opentelemetry::trace::Tracer> getTracer(std::string serviceName)
{
  auto provider = opentelemetry::trace::Provider::GetTracerProvider();
  return provider->GetTracer(serviceName);
}

void trace(std::string serviceName, std::string operationName)
{
  auto span = getTracer(serviceName)->StartSpan(operationName);

  span->SetAttribute("service.name", serviceName);

  // auto scope = getTracer(serviceName)->WithActiveSpan(span);

  // do something with span, and scope then...
  //
  span->End();
}